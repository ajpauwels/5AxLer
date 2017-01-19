//
//  Island.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Island.hpp"

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

Island::Island(const Polygon & polygon, vector<shared_ptr<const Mesh::Face>> p_polygonMeshFaces, bool isHole) :
m_polygon(polygon),
m_p_mainPolygonMeshFaces(p_polygonMeshFaces),
m_isHole(isHole) { }

const Polygon & Island::polygon() const {
    return m_polygon;
}

const vector<shared_ptr<const Mesh::Face>> & Island::mainPolygonMeshFaces() const {
    return m_p_mainPolygonMeshFaces;
}

vector<shared_ptr<const Mesh::Face>> Island::allFaces() const {
	vector<shared_ptr<const Mesh::Face>> faces;

	// Copy in the top level faces of the island
	for (shared_ptr<const Mesh::Face> p_face : m_p_mainPolygonMeshFaces) {
		faces.push_back(p_face);
	}

	// Get the child faces
	for (shared_ptr<Island> child : m_children) {
		vector<shared_ptr<const Mesh::Face>> childFaces = child->allFaces();

		for (shared_ptr<const Mesh::Face> p_childFace : childFaces) {
			faces.push_back(p_childFace);
		}
	}

	return faces;
}

const vector<shared_ptr<Island>> & Island::children() const {
    return m_children;
}

void Island::addChild(shared_ptr<Island> p_child) {
    p_child->m_p_parentIsland = shared_ptr<const Island>(this);
    m_children.push_back(p_child);
}

/** 
 * Converts the Island into a Polygon that matches its shape
 *
 * @param allPolys A vector to store the polygons in
 */
void Island::toPoly(vector<Polygon> & allPolys) const {
	allPolys.push_back(m_polygon);
	
	for (shared_ptr<Island> child : m_children) {
		child->toPoly(allPolys);
	}
}