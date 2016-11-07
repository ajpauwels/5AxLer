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
            ON,
            ABOVE,
            BELOW
        };
        
        Plane(const Vector3D & normal = Vector3D(0, 0, 1), const Vector3D & origin = Vector3D(0, 0, 0)); //normal vector set as default xy plane so plane will still have a normal when initialized with default value
        
        //getters
        const Vector3D & normal() const;
        const Vector3D & origin() const;
        
        //setters
        void normal(Vector3D normal);
        void origin(Vector3D origin);
        
        PLANE_POSITION pointOnPlane(const Vector3D & point) const;
        
        //static member's getters/setters
        static double faultTolerance();
        static void faultTolerance(double faultTolerance);
        
        std::string toString() const;
        
    private:
        Vector3D normal_, origin_;
        static double faultTolerance_; //used for fault tolerance when determining if a point is on polygon plane
    };
}

#endif /* Plane_hpp */
