//
//  Plane.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/5/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Plane_hpp
#define Plane_hpp

#include <string>

#include "Vector3D.hpp"

namespace mapmqp {
    class Plane {
    public:
        enum PLANE_POSITION {
            BELOW = -1,
            ON = 0,
            ABOVE = 1
        };
        
        Plane(const Vector3D & normal = Vector3D(0, 0, 1), double scalar = 0.0); //normal vector set as default xy plane so plane will still have a normal when initialized with default value
        
        //getters
        const Vector3D & normal() const;
        const double scalar() const;
        const Vector3D & origin() const;
        
        //setters
        void normal(Vector3D normal);
        void origin(Vector3D origin);
        
        PLANE_POSITION pointOnPlane(const Vector3D & point, double faultTolerance) const;
        PLANE_POSITION pointOnPlane(const Vector3D & point) const { return pointOnPlane(point, s_faultTolerance); }
        
        //static member's getters/setters
        static double faultTolerance();
        static void faultTolerance(double faultTolerance);
        
        std::string toString() const;
        
    private:
        Vector3D m_normal, m_origin;
        static double s_faultTolerance; //used for fault tolerance when determining if a point is on polygon plane
    };
}

#endif /* Plane_hpp */
