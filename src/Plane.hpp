//
//  Plane.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/5/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Plane_hpp
#define Plane_hpp

#include "Vector3D.hpp"

namespace mapmqp {
    class Plane {
    public:
        Plane(const Vector3D & normal = Vector3D(0, 0, 0), const Vector3D & origin = Vector3D(0, 0, 0));
        
        //getters
        const Vector3D & normal() const;
        const Vector3D & origin() const;
        
        //setters
        void normal(Vector3D normal);
        void origin(Vector3D origin);
        
        bool pointOnPlane(const Vector3D & point) const;
        
        //static member's getters/setters
        static double faultTolerance();
        static void faultTolerance(double faultTolerance);
        
    private:
        Vector3D normal_, origin_;
        static double faultTolerance_; //used for fault tolerance when determining if a point is on polygon plane
    };
}

#endif /* Plane_hpp */
