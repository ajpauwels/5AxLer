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

Island::Island(const Polygon & mainPolygon, vector<shared_ptr<const MeshFace>> p_mainPolygonMeshFaces) :
mainPolygon_(mainPolygon),
p_mainPolygonMeshFaces_(p_mainPolygonMeshFaces) { }

const Polygon & Island::mainPolygon() const {
    return mainPolygon_;
}

const vector<shared_ptr<const MeshFace>> & Island::p_mainPolygonMeshFaces() const {
    return p_mainPolygonMeshFaces_;
}

const vector<shared_ptr<Island::Hole>> & Island::holes() const {
    return holes_;
}

void Island::addHole(shared_ptr<Hole> p_hole) {
    p_hole->p_parentIsland_ = shared_ptr<const Island>(this);
    holes_.push_back(p_hole);
}

void Island::addIslandToHole(shared_ptr<const Island> p_island, unsigned int holeIndex) {
    if (holeIndex >= holes_.size()) {
        writeLog(WARNING, "attempting to add island to hole index that does not exist");
    } else {
        holes_[holeIndex]->p_holeIslands_.push_back(p_island);
    }
}

vector<shared_ptr<const Island>> Island::getP_SubIslandsAtDepth(unsigned int depth) {
    vector<shared_ptr<const Island>> ret;
    
    if (depth == 0) {
        ret.push_back(shared_ptr<const Island>(this));
        return ret;
    }
    
    for (vector<shared_ptr<Island::Hole>>::const_iterator it = holes_.begin(); it != holes_.end(); it++) {
        for (vector<shared_ptr<const Island>>::const_iterator subIt = (*it)->p_holeIslands_.begin(); subIt != (*it)->p_holeIslands_.end(); subIt++) {
            ret.push_back(*subIt);
        }
    }
    
    return ret;
}
