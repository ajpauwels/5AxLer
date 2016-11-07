//
//  Plane.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/5/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Plane.hpp"

#include <cmath>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

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

Plane::PLANE_POSITION Plane::pointOnPlane(const Vector3D & point) const {
    if (point == origin_) {
        return ON;
    }
    
    double dotVal = Vector3D::dotProduct(point - origin_, normal_);
    
    if (doubleEquals(dotVal, 0.0, faultTolerance_)) {
        return ON;
    } else if (dotVal > 0) {
        return ABOVE;
    } else {
        return BELOW;
    }
}

double Plane::faultTolerance() {
    return Plane::faultTolerance_;
}

void Plane::faultTolerance(double faultTolerance) {
    Plane::faultTolerance_ = faultTolerance;
}

string Plane::toString() const {
    ostringstream stream;
    stream << "[plane:" << normal_.toString() << ", origin:" << origin_.toString() << "]";
    return stream.str();
}
