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

#include "../libs/clipper/clipper.hpp"

#include "Vector3D.hpp"
#include "Plane.hpp"

namespace mapmqp {
    class Polygon {
    public:
        Polygon(const std::vector<Vector3D> & points); //assumes all points form one complete polygon on singular plane
        
        //getters
        const std::vector<Vector3D> & points() const;
        const Plane & plane() const;
        
        Polygon mapToXYPlane() const;
        Vector3D mapPointToXYPlane(const Vector3D & point) const;
        
        double area() const;
        
        bool pointOnPlane(const Vector3D & point) const;
        bool pointInPolygon(const Vector3D & point) const;
        
        //static member's getters/setters
        static uint64_t mappedPointPrecision();
        static void mappedPointPrecision(int mappedPointPrecision);
        
    private:
        std::vector<Vector3D> points_; //stored in counter-clockwise form
        Plane plane_;
        Vector3D planeAxisX_, planeAxisY_; //create x and y axes relative to polygon plane
        
        //TODO should these be variables of #defines?
        static uint64_t mappedPointPrecision_; //since clipper only uses integers as coordinates, coordinates are multiplied by mappedPointPrecision_ when stored in polygonXYPlane_ and divided back by mappedPointPrecision_ when returned (clipper integers range from +/- 4.6e18 ~= 2^62)
        
        ClipperLib::Path polygonXYPlane_; //clipper representation of polygon
        bool solved_; //whether or not polygon_ has been updated to represent polygon
    };
}

#endif /* Polygon_hpp */
