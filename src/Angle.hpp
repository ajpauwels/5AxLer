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
        Angle(double val = 0);
        
        // Getters
        double val() const;                     // Will be in range of [0, 2pi)
        double sinVal(double scale = 1) const;  // Returns sin(val) * scale
        double cosVal(double scale = 1) const;  // Returns cos(val) * scale
        
        // Setters
        void val(double val);
        
    private:
        double m_val;                                    // The angle value
        mutable double m_sin = 0, m_cos = 0;              // The sine and cosine values of the angle
        mutable bool m_sinSet = false, m_cosSet = false;  // True if the sine/cosine value has been computed already
    };
}

#endif /* Angle_hpp */
