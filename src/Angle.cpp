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

Angle::Angle(double val) {
    this->val(val);
}

double Angle::val() const {
    return val_;
}

double Angle::sinVal(double scale) const {
    if (!sinSet_) {
        sin_ = sin(val_);
        sinSet_ = true;
    }
    return sin_ * scale;
}

double Angle::cosVal(double scale) const {
    if (!cosSet_) {
        cos_ = cos(val_);
        cosSet_ = true;
    }
    return cos_ * scale;
}

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
