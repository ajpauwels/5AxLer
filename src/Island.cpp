//
//  Island.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Island.hpp"

using namespace mapmqp;
using namespace std;

Island::Island(Polygon mainPolygon, vector<shared_ptr<const MeshFace>> p_mainPolygonMeshFaces) :
mainPolygon_(mainPolygon),
p_mainPolygonMeshFaces_(p_mainPolygonMeshFaces) { }

const Polygon & Island::mainPolygon() {
    return mainPolygon_;
}

const vector<shared_ptr<const MeshFace>> & Island::p_mainPolygonMeshFaces() {
    return p_mainPolygonMeshFaces_;
}

const vector<Polygon> & Island::holes() {
    return holes_;
}

const vector<vector<shared_ptr<const MeshFace>>> & Island::p_holesMeshFaces() {
    return p_holesMeshFaces_;
}

void Island::addHole(Polygon hole, vector<shared_ptr<const MeshFace>> p_holeMeshFaces) {
    holes_.push_back(hole);
    p_holesMeshFaces_.push_back(p_holeMeshFaces);
}
