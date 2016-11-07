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

Slice::Slice(const Plane & slicePlane) :
slicePlane_(slicePlane) { }

const Plane & Slice::slicePlane() const {
    return slicePlane_;
}

const std::vector<Island> & Slice::islands() const {
    return islands_;
}

void Slice::addIsland(Island island) {
    islands_.push_back(island);
}
