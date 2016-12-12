//
//  VolumeDecomposer.cpp
//  5AxLer
//
//  Created by Alexandre Pauwels on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include <random>
#include "VolumeDecomposer.hpp"

using namespace mapmqp;
using namespace std;

// Random number generation
std::random_device rd;		// only used once to initialise (seed) engine
std::mt19937 rng(rd());		// random-number engine used (Mersenne-Twister in this case)

VolumeDecomposer::VolumeDecomposer() {}

/**
 * Takes a pointer to the mesh and the orientation to begin processing
 * the mesh in, and decomposes the object into overhang and buildable parts
 *
 * @param p_mesh A pointer to the Mesh
 * @param orientation A Plane object representing the orientation to begin slicing in
 */
vector<shared_ptr<Mesh>> VolumeDecomposer::run(shared_ptr<Mesh> p_mesh, Plane orientation) {
	// Initializes the slicer for the decomposition
	Slicer slicer = new Slicer(p_mesh);

	// The two slice planes that will be used, one on top of the other
	Plane firstPlane = new Plane(orientation, i * SLICE_THICKNESS);
	Plane secondPlane = new Plane(orientation, (i + 1) * SLICE_THICKNESS);

	// The comparison slice used to determine the buildable portions of the model
	Slicer::Slice comparisonSlice = slicer.slice(firstPlane);

	// The current slice being processed for overhangs and buildable parts
	Slicer::Slice currSlice = slicer.slice(secondPlane);

	// The polygonal representations of both slices
	Polygons comparisonPoly = comparisonSlice.toPoly();
	Polygons slicePoly;

	// Iterate through the whole height of the object
	for (int i = 0; i < 1000; ++i) {
		// Get the current slice's polygonal representation
		slicePoly = currSlice.toPoly();

		// Get the overhangs by subtracting the comparison from the current slice
		Polygons overhangsPoly = slicePoly - comparisonPoly;

		// Check if this is a radial overhang, and if so update the comparisonPoly to use
		// Radial overhang = top slice completely encompasses comparison poly
		double allOverhangsPolyArea = overhangsPoly.area();
		double comparisonPolyArea = comparisonPoly.area();
		double slicePolyArea = slicePoly.area();

		if (allOverhangsPolyArea == slicePolyArea - comparisonPolyArea) {
			overhangsPoly = slicePoly - expandedComparisonPoly;
			comparisonPoly = expandedComparisonPoly;

			allOverhangsPolyArea = overhangsPoly.area();
			comparisonPolyArea = comparisonPoly.area();

			// Check if it's a complete radial overhang, and if so, bail out
			// Complete radial overhang = radial overhang with additional restriction that the entirety
			// of the top slice must be an overhang (no 45 degree overhangs)
			if (allOverhangsPolyArea == slicePolyArea - comparisonPolyArea) {
				writeLog(ERROR, "attempting to slice a complete radial overhang, quitting");
				exit(0);
			}
		}

		// Go through each non-overhang polygon to process its overhanging parts
		for (Polygon poly : overhangsPoly) {
			// TODO: if the poly is a hole, should it be added to the comparison slice?
			if (poly.area() > 0 && isOverhang(poly, comparisonPoly)) {
				processOverhang(poly, comparisonPoly, currSlice, slicePoly);
			}
		}

		// Update the comparison poly
		comparisonPoly = intersection(comparisonPoly, slicePoly);
	}
}

/**
 * Checks to see if a polygon is an overhang or an
 * acceptable buildable area
 *
 * @param checkPoly The polygon that's being checked
 * @param comparisonPoly The group of polygons below the checkPoly that will be used to check overhang
 *
 * @return True if checkPoly is an overhang, false otherwise
 */
bool VolumeDecomposer::isOverhang(Polygon checkPoly, Polygons comparisonPoly) {
	// Expand the comparison polygon to fit a 45 degree overhang
	Polygons expanded = comparisonPoly.expand(45);

	// Subtract the area of the polygon from the comparison
	Polygons subtracted = expanded - checkPoly;

	// If the area removed from the comparison is exactly equal to the area of the
	// polygon, then that polygon is totally inside the expanded comparison and is
	// not an overhang
	if (subtracted.area() == (expanded.area() - checkPoly.area())) {
		return false;
	}

	return true;
}

/**
 * Takes an overhang polygon, the comparison polygon, and the slice that the overhang
 * is on and processes the slice faces into buildable and overhang meshes.
 */
void VolumeDecomposer::processOverhang(Polygon overhangPoly, Polygons comparisonPoly, Slicer::Slice slice, Polygons slicePoly) {
	// Point at index 0 on the overhang polygon
	vector<Vector3D> overhangPts = overhangPoly.points();
	Vector3D ptOverhang = overhangPts.get(0);
	vector<Vector3D>::iterator it;

	// Index of the point on the overhang
	int idxOverhang = 0;

	// Try and find the point that's on the overhang on the comparison polygon, and get its index if so
	int idxComparison = getPointIndex(ptOverhang, comparisonPoly);

	// Same as above but on the current slice polygon
	int idxSlice = getPointIndex(ptOverhang, slicePoly);

	// The polygon that we're currently iterating through
	Polygons iteratingPoly;

	// The index we're in the polygon we're iterating through
	int iteratingIdx;

	// True if the iterating poly is the comparison poly, false otherwise
	bool usingComparison;

	// Case where the point we start off with is an intersection
	if (idxComparison != -1 && idxSlice != -1) {

	}
	// Case where the point is on the interior part of the overhang
	else if (idxComparison != -1) {
		iteratingPoly = comparisonPoly;
		iteratingIdx = idxComparison;
		usingComparison = true;
	}
	// Case where the point is on the exterior part of the overhang
	else if (idxSlice != -1) {
		iteratingPoly = slicePoly;
		iteratingIdx = idxSlice;
		usingComparison = false;
	}
	// Case where the point is somewhere else and something has gone wrong
	else {
		writeLog(ERROR, "a point on the overhang was on neither the current or comparison slice");
		return;
	}

	// Loop through all overhang points to add an extra point between each pair
	// This is to avoid issues where there are no points on the overhang that are not on
	// the slice (which occurs when the interior part of the overhang is a straight line)
	vector<Vector3D> newPts;
	for (int i = 0; i < overhangPts.size() - 1; ++i) {
		Vector3D pt1 = overhangPts.get(i);
		Vector3D pt2 = overhangPts.get(i + 2);

		// Get the slope of the line connecting the pair and x range of the points
		// TODO: Random numbers are ints and x/y are floats, fix this
		// TODO: We're doing all of this in 2D but the points are 3D...
		float slope = (pt2.y() - pt1.y()) / (pt2.x() - pt1.x());
		float minX = pt1.x() > pt2.x() ? pt2.x() : pt1.x();
		float maxX = pt1.x() > pt2.x() ? pt1.x() : pt2.x();

		// Randomly choose an x in the range
		std::uniform_int_distribution<int> uni(minX,maxX);	// guaranteed unbiased
		int randX = uni(rng);

		// Get the random y from the random x and the slope
		int randY = slope * (randX - pt1.x()) + pt1.y();

		Vector3D newPt = new Vector3D(randX, randY, 0);
		newPts.push_back(newPt);
	}
	overhangPts.insert(overhangPts.end(), newPts.begin(), newPts.end());

	// Loop through all overhang points
	for (int i = 1; i < overhangPts.size(); ++i) {
		// Retrieve the point we're searching for
		Vector3D pt = overhangPts.get(i);

		// Iterate through the polygon until pt is found
		iteratingIdx = iterateToPoint(pt, iteratingPoly, iteratingIdx);

		// If we couldn't find the point, flip which poly to iterate over
		if (iteratingIdx = -1) {
			iteratingPoly = usingComparison ? slicePoly : comparisonPoly;
			usingComparison ^= 1;
		}

		// Remove the face from the graph

		// Split the face into three triangles

		// Add the 1 or 2 overhang faces to the entry of overhang faces
	}
}