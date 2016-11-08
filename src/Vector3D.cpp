//
//  Vector3D.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Vector3D.hpp"

#include <cmath>
#include <iostream>
#include <sstream>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

double Vector3D::s_faultTolerance = 0.0;

Vector3D::Vector3D(double x, double y, double z, bool unitVector) :
m_x(x),
m_y(y),
m_z(z) {
    if (unitVector) {
        normalize();
    }
}

Vector3D::Vector3D(Angle theta, Angle phi, double r) :
m_x(r * phi.sinVal() * theta.cosVal()),
m_y(r * phi.sinVal() * theta.sinVal()),
m_z(r * phi.cosVal()) { }

double Vector3D::x() const {
    return m_x;
}

double Vector3D::y() const {
    return m_y;
}

double Vector3D::z() const {
    return m_z;
}

Angle Vector3D::theta() const {
    return Angle(atan2(m_y, m_x));
}

Angle Vector3D::phi() const {
    double xyDistance = sqrt(m_x * m_x + m_y * m_y);
    if (xyDistance == 0) {
        return (m_z < 0) ? Angle(-M_PI) : Angle(0);
    } else {
        return Angle(atan2(sqrt(m_x * m_x + m_y * m_y), m_z));
    }
}

double Vector3D::magnitude() const {
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

double Vector3D::faultTolerance() {
    return Vector3D::s_faultTolerance;
}

void Vector3D::x(double x) {
    m_x = x;
}

void Vector3D::y(double y) {
    m_y = y;
}

void Vector3D::z(double z) {
    m_z = z;
}

void Vector3D::theta(Angle theta) {
    double r = magnitude();
    Angle phi = this->phi();
    m_x = r * phi.sinVal() * theta.cosVal();
    m_y = r * phi.sinVal() * theta.sinVal();
}

void Vector3D::phi(Angle phi) {
    double r = magnitude();
    Angle theta = this->theta();
    m_x = r * phi.sinVal() * theta.cosVal();
    m_y = r * phi.sinVal() * theta.sinVal();
    m_z = r * phi.cosVal();
}

void Vector3D::normalize(double value) {
    double magnitude = this->magnitude();
    if (magnitude != 0) {
        m_x = m_x * value / magnitude;
        m_y = m_y * value / magnitude;
        m_z = m_z * value / magnitude;
    } else {
        writeLog(WARNING, "attempted to normalize vector of magnitude 0");
    }
}

void Vector3D::faultTolerance(double faultTolerance) {
    Vector3D::s_faultTolerance = s_faultTolerance;
}

bool Vector3D::equals(const Vector3D & v, double faultTolerance) const {
    Vector3D dV = v - *this;

    return (fabs(dV.magnitude()) <= faultTolerance);
}

double Vector3D::dotProduct(const Vector3D & v1, const Vector3D & v2) {
    return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

Vector3D Vector3D::crossProduct(const Vector3D & v1, const Vector3D & v2) {
    return Vector3D(v1.y() * v2.z() - v1.z() * v2.y(), v1.z() * v2.x() - v1.x() * v2.z(), v1.x() * v2.y() - v1.y() * v2.x());
}

Vector3D Vector3D::operator+(const Vector3D & v) const {
    return Vector3D(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
}

Vector3D Vector3D::operator-(const Vector3D & v) const {
    return Vector3D(m_x - v.x(), m_y - v.y(), m_z - v.z());
}

Vector3D Vector3D::operator*(const double & scale) const {
    return Vector3D(m_x * scale, m_y * scale, m_z * scale);
}

Vector3D Vector3D::operator/(const double & scale) const {
    return Vector3D(m_x / scale, m_y / scale, m_z / scale);
}

bool Vector3D::operator==(const Vector3D & v) const {
    return equals(v, Vector3D::s_faultTolerance);
}

bool Vector3D::operator!=(const Vector3D & v) const {
    return !(*this == v);
}

string Vector3D::toString() const {
    ostringstream stream;
    stream << "[" << m_x << ", " << m_y << ", " << m_z << "]";
    return stream.str();
}
