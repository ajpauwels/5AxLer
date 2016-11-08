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

Island::Island(const Polygon & polygon, vector<shared_ptr<const MeshFace>> p_polygonMeshFaces) :
m_polygon(polygon),
m_p_mainPolygonMeshFaces(p_polygonMeshFaces) { }

const Polygon & Island::polygon() const {
    return m_polygon;
}

const vector<shared_ptr<const MeshFace>> & Island::p_mainPolygonMeshFaces() const {
    return m_p_mainPolygonMeshFaces;
}

const vector<shared_ptr<Island::Hole>> & Island::holes() const {
    return m_holes;
}

void Island::addHole(shared_ptr<Hole> p_hole) {
    p_hole->m_p_parentIsland = shared_ptr<const Island>(this);
    m_holes.push_back(p_hole);
}

void Island::addIslandToHole(shared_ptr<const Island> p_island, unsigned int holeIndex) {
    if (holeIndex >= m_holes.size()) {
        writeLog(WARNING, "attempting to add island to hole index that does not exist");
    } else {
        m_holes[holeIndex]->m_p_islands.push_back(p_island);
    }
}

vector<shared_ptr<const Island>> Island::getP_SubIslandsAtDepth(unsigned int depth) {
    vector<shared_ptr<const Island>> ret;
    
    if (depth == 0) {
        ret.push_back(shared_ptr<const Island>(this));
        return ret;
    }
    
    for (vector<shared_ptr<Island::Hole>>::const_iterator it = m_holes.begin(); it != m_holes.end(); it++) {
        for (vector<shared_ptr<const Island>>::const_iterator subIt = (*it)->m_p_islands.begin(); subIt != (*it)->m_p_islands.end(); subIt++) {
            ret.push_back(*subIt);
        }
    }
    
    return ret;
}
