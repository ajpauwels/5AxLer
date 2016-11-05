//
//  Slice.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Slice.hpp"

using namespace mapmqp;
using namespace std;

Slice::Slice(Vector3D slicePlaneNormal, Vector3D slicePlaneOrigin) :
slicePlaneNormal_(slicePlaneNormal),
slicePlaneOrigin_(slicePlaneOrigin) { }

const std::vector<Island> & Slice::islands() const {
    return islands_;
}

Vector3D Slice::slicePlaneNormal() const {
    return slicePlaneNormal_;
}

Vector3D Slice::slicePlaneOrigin() const {
    return slicePlaneOrigin_;
}

void Slice::addIsland(Island island) {
    islands_.push_back(island);
}
