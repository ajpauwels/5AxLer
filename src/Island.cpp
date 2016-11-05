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

const vector<Polygon> & Island::holes() const {
    return holes_;
}

const vector<vector<shared_ptr<const MeshFace>>> & Island::p_holesMeshFaces() const {
    return p_holesMeshFaces_;
}

void Island::addHole(Polygon hole, vector<shared_ptr<const MeshFace>> p_holeMeshFaces) {
    holes_.push_back(hole);
    p_holesMeshFaces_.push_back(p_holeMeshFaces);
    holeIslands_.resize(holes_.size());
}

void Island::addIslandToHole(Island island, int holeIndex) {
    if (holeIndex < 0) {
        writeLog(WARNING, "attempting to add island to hole with index less than 0");
    } else if (holeIndex >= holeIslands_.size()) {
        writeLog(WARNING, "attempting to add island to hole index that does not exist");
    } else {
        holeIslands_[holeIndex].push_back(island);
    }
}
