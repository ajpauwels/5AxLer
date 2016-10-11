//
//  Vector3D.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Vector3D.hpp"

#include <cmath>

#include "Utility.hpp"

using namespace mapmqp;

Vector3D::Vector3D(double x, double y, double z, bool unitVector) :
x_(x),
y_(y),
z_(z) {
    if (unitVector) {
        normalize();
    }
}

Vector3D::Vector3D(Angle theta, Angle phi, double r) :
x_(r * phi.sinVal() * theta.cosVal()),
y_(r * phi.sinVal() * theta.sinVal()),
z_(r * phi.cosVal()) { }

double Vector3D::x() const {
    return x_;
}

double Vector3D::y() const {
    return y_;
}

double Vector3D::z() const {
    return z_;
}

Angle Vector3D::theta() const {
    return Angle(atan2(y_, x_));
}

Angle Vector3D::phi() const {
    double xyDistance = sqrt(x_ * x_ + y_ * y_);
    if (xyDistance == 0) {
        return (z_ < 0) ? Angle(-M_PI) : Angle(0);
    } else {
        return Angle(atan2(sqrt(x_ * x_ + y_ * y_), z_));
    }
}

double Vector3D::magnitude() const {
    return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

void Vector3D::x(double x) {
    x_ = x;
}

void Vector3D::y(double y) {
    y_ = y;
}

void Vector3D::z(double z) {
    z_ = z;
}

void Vector3D::theta(Angle theta) {
    double r = magnitude();
    Angle phi = this->phi();
    x_ = r * phi.sinVal() * theta.cosVal();
    y_ = r * phi.sinVal() * theta.sinVal();
}

void Vector3D::phi(Angle phi) {
    double r = magnitude();
    Angle theta = this->theta();
    x_ = r * phi.sinVal() * theta.cosVal();
    y_ = r * phi.sinVal() * theta.sinVal();
    z_ = r * phi.cosVal();
}

void Vector3D::normalize(double value) {
    double total = fabs(x_) + fabs(y_) + fabs(z_);
    if (total != 0) {
        x_ = x_ * value / total;
        y_ = y_ * value / total;
        z_ = z_ * value / total;
    } else {
        writeLog(WARNING_MESSAGE, "attempted to normalize vector of magnitude 0");
    }
}

double Vector3D::dotProduct(Vector3D v1, Vector3D v2) {
    return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

Vector3D Vector3D::crossProduct(Vector3D v1, Vector3D v2) {
    return Vector3D(v1.y() * v2.z() - v1.z() * v2.y(), v1.z() * v2.x() - v1.x() * v2.z(), v1.x() * v2.y() - v1.y() * v2.x());
}

Vector3D Vector3D::operator+(const Vector3D & v) const {
    return Vector3D(x_ + v.x_, y_ + v.y_, z_ + v.z_);
}

Vector3D Vector3D::operator-(const Vector3D & v) const {
    return Vector3D(x_ - v.x_, y_ - v.y_, z_ - v.z_);
}

Vector3D Vector3D::operator*(const double & scale) const {
    return Vector3D(x_ * scale, y_ * scale, z_ * scale);
}

Vector3D Vector3D::operator/(const double & scale) const {
    return Vector3D(x_ / scale, y_ / scale, z_ / scale);
}

bool Vector3D::operator==(const Vector3D & v) const {
    return (x_ == v.x_) && (y_ == v.y_) && (z_ == v.z_);
}

bool Vector3D::operator!=(const Vector3D & v) const {
    return !(*this == v);
}
