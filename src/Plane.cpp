//
//  Plane.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/5/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Plane.hpp"

#include <cmath>

using namespace mapmqp;

double Plane::faultTolerance_ = 0.0;

Plane::Plane(const Vector3D & normal, const Vector3D & origin) :
normal_(normal),
origin_(origin) { }

const Vector3D & Plane::normal() const {
    return normal_;
}

const Vector3D & Plane::origin() const {
    return origin_;
}

void Plane::normal(Vector3D normal) {
    normal_ = normal;
}

void Plane::origin(Vector3D origin) {
    origin_ = origin;
}

bool Plane::pointOnPlane(const Vector3D & point) const {
    return ((point == origin_) || (fabs(Vector3D::dotProduct(point - origin_, normal_)) <= Plane::faultTolerance_));
}

double Plane::faultTolerance() {
    return Plane::faultTolerance_;
}

void Plane::faultTolerance(double faultTolerance) {
    Plane::faultTolerance_ = faultTolerance;
}
