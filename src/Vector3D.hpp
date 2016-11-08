//
//  Vector3D.hpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Vector3D_hpp
#define Vector3D_hpp

#include <string>

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
        
        bool equals(const Vector3D & v, double faultTolerance) const;
        bool equals(const Vector3D & v) const { return equals(v, Vector3D::s_faultTolerance); }
        
        //static member's getters/setters
        static double faultTolerance();
        static void faultTolerance(double faultTolerance);
        
        static double dotProduct(const Vector3D & v1, const Vector3D & v2);
        static Vector3D crossProduct(const Vector3D & v1, const Vector3D & v2);
        
        Vector3D operator+(const Vector3D & v) const;
        Vector3D operator-(const Vector3D & v) const;
        Vector3D operator*(const double & scale) const;
        Vector3D operator/(const double & scale) const;
        
        bool operator==(const Vector3D & v) const; //calls equals function
        bool operator!=(const Vector3D & v) const;

        std::string toString() const;
        
    private:
        double m_x, m_y, m_z;
        static double s_faultTolerance;
    };
}

#endif /* Vector3D_hpp */
