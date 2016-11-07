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

const std::vector<shared_ptr<const Island>> & Slice::p_islands() const {
    return p_islands_;
}

void Slice::addIsland(shared_ptr<const Island> p_island) {
    p_islands_.push_back(p_island);
}
