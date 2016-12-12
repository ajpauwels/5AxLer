//
//  Angle.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Angle.hpp"

#include <sstream>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

double Angle::s_faultTolerance = 0.0;

/**
 * Constructor builds an angle out of the given value,
 * defaults to 0 radians if no angle given.
 *
 * @param val A double giving the value of the angle (in radians)
 */
Angle::Angle(double val) {
    this->val(val);
}

/**
 * Return the value of this angle (in radians).
 *
 * @return A double giving the value of the angle (in radians)
 */
double Angle::val() const {
    return m_val;
}

/**
 * Returns the sine of the angle value (in radians), multiplied by the
 * given scale. If no scale is given, it defaults to 1.
 * This function computes the sine only once and from then
 * on returns the pre-computed value.
 *
 * @param scale The scale to multiply the sine by, defaults to 1
 * @return A double giving the sine value of the angle (in radians)
 */
double Angle::sinVal(double scale) const {
    if (!m_sinSet) {
        m_sin = sin(m_val);
        m_sinSet = true;
    }
    return m_sin * scale;
}

/**
 * Returns the cosine of the angle value (in radianss), multiplied by the
 * given scale. If no scale is given, it defaults to 1.
 * This function computes the cosine only once and from then
 * on returns the pre-computed value.
 *
 * @param scale The scale to multiply the cosine by, defaults to 1
 * @return A double giving the cosine value of the angle (in radians)
 */
double Angle::cosVal(double scale) const {
    if (!m_cosSet) {
        m_cos = cos(m_val);
        m_cosSet = true;
    }
    return m_cos * scale;
}

/**
 * Sets the angle's value to a new value. Note that
 * all degree measures are reduced down to be between
 * 0 and 2 * PI and the value returned by val() will be
 * in this range, your original angle will be lost.
 *
 * @param val The new angle value to use (in radians)
 */
void Angle::val(double val) {
    //confirms val is in range [0, 2*pi)
    while (val >= (2.0 * M_PI)) {
        val -= (2.0 * M_PI);
    }
    while (val < 0) {
        val += (2.0 * M_PI);
    }
    
    m_val = val;
    m_sinSet = m_cosSet = false;
}

bool Angle::equals(const Angle & angle, double faultTolerance) const {
    return doubleEquals(m_val, angle.val(), faultTolerance);
}

Angle Angle::operator+(const Angle & angle) const {
    return Angle(m_val + angle.m_val);
}

Angle Angle::operator-(const Angle & angle) const {
    return Angle(m_val - angle.m_val);
}

Angle Angle::operator+(double angleVal) const {
    return Angle(m_val + angleVal);
}

Angle Angle::operator-(double angleVal) const {
    return Angle(m_val - angleVal);
}

bool Angle::operator==(const Angle & angle) const {
    return equals(angle, s_faultTolerance);
}

bool Angle::operator!=(const Angle & angle) const {
    return !equals(angle, s_faultTolerance);
}

std::string Angle::toString(bool degrees) const {
    ostringstream stream;
    if (degrees) {
        stream << radiansToDegrees(m_val) << "°";
    } else {
        stream << m_val;
    }
    return stream.str();
}
