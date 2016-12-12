//
//  Angle.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Angle_hpp
#define Angle_hpp

#include <cmath>
#include <string>

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
        
        bool equals(const Angle & angle, double faultTolerance) const;
        bool equals(const Angle & angle) const { return equals(angle, s_faultTolerance); }
        
        //static member's getters/setters
        static double faultTolerance();
        static void faultTolerance(double faultTolerance);
        
        static double radiansToDegrees(double radians) { return radians * 180.0 / M_PI; }
        static double degreesToRadians(double degrees) { return degrees * M_PI / 180.0; }
        
        Angle operator+(const Angle & angle) const;
        Angle operator-(const Angle & angle) const;
        Angle operator+(double angleVal) const;
        Angle operator-(double angleVal) const;
        
        bool operator==(const Angle & angle) const; //calls equals
        bool operator!=(const Angle & angle) const;
        
        std::string toString(bool degrees = true) const;
        
    private:
        double m_val;                                    // The angle value
        mutable double m_sin = 0, m_cos = 0;              // The sine and cosine values of the angle
        mutable bool m_sinSet = false, m_cosSet = false;  // True if the sine/cosine value has been computed already
        
        static double s_faultTolerance;
    };
}

#endif /* Angle_hpp */
