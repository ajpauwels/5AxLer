//
//  VolumeDecomposer.cpp
//  5AxLer
//
//  Created by Alexandre Pauwels on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "VolumeDecomposer.hpp"

using namespace mapmqp;
using namespace std;

vector<shared_ptr<Mesh>> run(shared_ptr<Mesh> p_mesh, Plane orientation) {
	// Initializes the slicer for the decomposition
	Slicer slicer = new Slicer(p_mesh);

	// The two slice planes that will be used, one on top of the other
	Plane firstPlane = new Plane(orientation, 0);

	// The comparison slice used to determine the buildable portions of the model
	Slicer::Slice comparisonSlice = slicer.slice(firstPlane);

	// The current slice being processed for overhangs and buildable parts
	Slicer::Slice currSlice;

	// The polygonal representations of both slices
	Polygons comparisonPoly = comparisonSlice.toPoly();

	// TODO: Update the slicer to have hasNextSlice(), nextSlice(), and plane()
	// Go through all the slices
	while (slicer.hasNextSlice()) {
		// Retrieve the slice
		currSlice = slicer.nextSlice();

		// Go through each face in the slice
		for (std::shared_ptr<const Mesh::Face>> mf : currSlice.faces()) {
			// Check that all points of the face are inside the comparison poly
			bool allInsideComparison = true;
			bool allOutsideComparison = true;
			vector<Vector3D> flattenedPoints;

			for (int i = 0; i < 3 && allInsideComparison; ++i) {
				// TODO: write a dropVertexToPlane function that drops each vertex of a face
				// onto the plane along the normal of the plane
				Vector3D pt = dropVertexToPlane(mf.p_vertex(i).vertex(), slicer.plane());

				// Check if the point is in the polygon and update the flags
				// A point directly on the polygon boundary is considered outside
				bool inPoly = comparisonPoly.pointInPolygon(p_vertex.vertex());
				allInsideComparison &= (inPoly == 1);
				allOutsideComparison &= (inPoly <= 0);

				// Add the point to our vector of flattened points
				flattenedPoints.push_back(pt);
			}

			// TODO: Figure out a better way to handle fatal errors
			// Error case where the vertices of the triangle are all both in and out of the comparison poly
			if (allInsideComparison && allOutsideComparison) {
				writeLog(ERROR, "all of the vertices of a triangle are both inside and outside the comparison polygon")
				exit(0);
			}
			// Second case where the triangle has some vertices inside and some vertices outside the polygon
			else if (!allInsideComparison && !allOutsideComparison) {
				// First, check to see if the comparison polygon and triangle are parallel to each other
				bool parallel = areParallel(comparisonPoly, mf);
				vector<std::shared_ptr<Mesh::Face>> newFaces;

				// If they are not parallel, we can find the intersection between comparison poly and face
				if (!parallel) {
					// TODO: Write function that takes the comparison poly and the mesh face
					// and returns the <x, y, z> intersecting point
					Vector3D intersection = findPolyFaceIntersection(comparisonPoly, mf);

					// TODO: Write a function that takes a point on the face and a split direction
					// and splits the face along that direction, creating three new faces
					newFaces = splitFaceAlongDirection(mf, intersection, plane.normal());
				}
				// However if they are parallel, we must cut the triangle with the slice
				else {
					// TODO: Write a function that takes the face and the comparison polygon,
					// intersects them, 
					newFaces = splitFaceAlongSlice(mf, comparisonPoly);
				}
			}
		}
	}
}

/**
 * findPolyFaceIntersection is fairly straightforward:
 * 1. Iterate through each line segment in the polygon
 * 2. Take the mesh face and the two points of the line and pass them through the following equations:
 *		d = (n_x * x_0) + (n_y * y_0) + (n_z * z_0)
 *		t = (d - x_0 - y_0 - z_0) / (deltaX + deltaY + deltaZ)
 *		
 *		d is part of the scalar equation of the plane
 *		<n_x, n_y, n_z> are the face's normal
 *		<x_0, y_0, z_0> in the top equation is the <x, y, z> of one of the face's vertices
 *		<x_0, y_0, z_0> in the bottom equation is the <x, y, z> of one of the endpoints of the line
 *		deltaX = x_1 - x_0, where x_1 is the other endpoint of the line's x-coordinate (etc for deltaY, deltaZ)
 * 3. Now we check t. If 0 <= t <= 1, then the line from the comparison poly intersects the face
 * 4. In that case, we plug t into the following equation to get the final intersection's <x, y, z>:
 *		r(t) = <x_0 + deltaX*t, y_0 + deltaY*t, z_0 + deltaZ*t>
 *		<x_0, y_0, z_0> is the first endpoint of the line
 *		deltaX etc are defined as above
 * 5. Otherwise, the line does not intersect and we move on to the next line
 * 6. If no intersections are found, an exception is thrown
 */