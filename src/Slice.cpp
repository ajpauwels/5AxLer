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
m_slicePlane(slicePlane) { }

const Plane & Slice::slicePlane() const {
    return m_slicePlane;
}

const std::vector<shared_ptr<const Island>> & Slice::p_islands() const {
    return m_p_islands;
}

void Slice::addIsland(shared_ptr<const Island> p_island) {
    m_p_islands.push_back(p_island);
}
