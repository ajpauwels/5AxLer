//
//  Vector3D.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Vector3D_hpp
#define Vector3D_hpp

#include "Angle.hpp"

namespace mapmqp {
    class Vector3D {
    public:
        Vector3D(double x = 0, double y = 0, double z = 0, bool unitVector = false);
        Vector3D(Angle theta, Angle phi, double magntiude = 1);
        
        //getters
        double x() const;
        double y() const;
        double z() const;
        Angle theta() const;
        Angle phi() const;
        double magnitude() const;
        
        //setters
        void x(double x);
        void y(double y);
        void z(double z);
        void theta(Angle theta);
        void phi(Angle phi);
        void normalize(double magnitude = 1);
        
        static double dotProduct(const Vector3D & v1, const Vector3D & v2);
        static Vector3D crossProduct(const Vector3D & v1, const Vector3D & v2);
        
        Vector3D operator+(const Vector3D & v) const;
        Vector3D operator-(const Vector3D & v) const;
        Vector3D operator*(const double & scale) const;
        Vector3D operator/(const double & scale) const;
        
        bool operator==(const Vector3D & v) const;
        bool operator!=(const Vector3D & v) const;
        
    private:
        double x_, y_, z_;
    };
}

#endif /* Vector3D_hpp */
