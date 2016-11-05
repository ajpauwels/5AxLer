//
//  Polygon.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/1/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Polygon_hpp
#define Polygon_hpp

#include <vector>

#include "Vector3D.hpp"
#include "../libs/clipper/clipper.hpp"

namespace mapmqp {
    class Polygon {
    public:
        Polygon(const std::vector<Vector3D> & points); //assumes all points form one complete polygon on singular plane
        
        //getters
        const std::vector<Vector3D> & points() const;
        Vector3D planeNormal() const;
        
        Polygon mapToXYPlane() const;
        Vector3D mapPointToXYPlane(const Vector3D & point) const;
        
        double area() const;
        
        bool pointOnPlane(const Vector3D & point) const;
        bool pointInPolygon(const Vector3D & point) const;
        
        //static member's getters/setters
        static double planeFaultTolerance();
        static void planeFaultTolerance(double planeFaultTolerance);
        static uint64_t mappedPointPrecision();
        static void mappedPointPrecision(int mappedPointPrecision);
        
    private:
        std::vector<Vector3D> points_; //stored in counter-clockwise form
        Vector3D planeNormal_; //normal of plane polygon lies on - determined by first three points added
        Vector3D planeOrigin_; //point on plane closest to (0, 0, 0)
        Vector3D planeAxisX_, planeAxisY_; //create x and y axes relative to polygon plane
        
        //TODO should these be variables of #defines?
        static double planeFaultTolerance_; //used for fault tolerance when determining if a point is on polygon plane
        static uint64_t mappedPointPrecision_; //since clipper only uses integers as coordinates, coordinates are multiplied by mappedPointPrecision_ when stored in polygonXYPlane_ and divided back by mappedPointPrecision_ when returned (clipper integers range from +/- 4.6e18 ~= 2^62)
        
        ClipperLib::Path polygonXYPlane_; //clipper representation of polygon
        bool solved_; //whether or not polygon_ has been updated to represent polygon
    };
}

#endif /* Polygon_hpp */
