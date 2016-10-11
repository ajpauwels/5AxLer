//
//  Angle.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Angle_hpp
#define Angle_hpp

namespace mapmqp {
    class Angle {
    public:
        Angle(double val);
        
        //getters
        double val() const; //will be in range of [0, 2pi)
        double sinVal(double scale = 1) const; //returns sin(val) * scale
        double cosVal(double scale = 1) const; //returns cos(val) * scale
        
        //setters
        void val(double val);
        
    private:
        double val_;
        mutable double sin_ = 0, cos_ = 0;
        mutable bool sinSet_ = false, cosSet_ = false;
    };
}

#endif /* Angle_hpp */
