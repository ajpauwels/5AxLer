//
//  Angle.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Angle.hpp"

#include <cmath>

using namespace mapmqp;

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
    return val_;
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
    if (!sinSet_) {
        sin_ = sin(val_);
        sinSet_ = true;
    }
    return sin_ * scale;
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
    if (!cosSet_) {
        cos_ = cos(val_);
        cosSet_ = true;
    }
    return cos_ * scale;
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
    
    val_ = val;
    sinSet_ = cosSet_ = false;
}
