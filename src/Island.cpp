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

const vector<shared_ptr<const Mesh::Face>> & Island::p_mainPolygonMeshFaces() const {
    return m_p_mainPolygonMeshFaces;
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
void Island::toPoly(std::vector<Polygon> & allPolys) {
	allPolys.push_back(m_polygon);
	
	for (std::shared_ptr<Island> child : m_children) {
		child.toPoly(allPolys);
	}
}