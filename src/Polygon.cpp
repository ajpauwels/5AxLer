//
//  Polygon.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/1/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Polygon.hpp"

#include <cmath>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

//see http://www.angusj.com/delphi/clipper/documentation/Docs/Overview/_Body.htm

uint64_t Polygon::mappedPointPrecision_ = 1000000;

Polygon::Polygon(const vector<Vector3D> & points) :
points_(points) {
    if (points.size() < 3) {
        writeLog(ERROR, "attempted to create polygon with only 2 points");
        //TODO throw exception
    }
    
    //TODO first 3 points may be on same line, in which case plane normal cannot be determined by first 3 points
    //set plane's normal
    Vector3D planeNormal = Vector3D::crossProduct(points[1] - points[0], points[2] - points[0]); //TODO this is backwards?
    planeNormal.normalize();
    
    //find closest point on plane to origin
    Vector3D planeOrigin = planeNormal;
    double t = Vector3D::dotProduct(planeNormal, points_[0]) / Vector3D::dotProduct(planeNormal, planeNormal);
    planeOrigin.normalize(t);
    
    plane_ = Plane(planeNormal, planeOrigin);
    
    //find arbitrary x and y axes of polygon plane such that dot(x, planeNormal) = dot(y, planeNormal) = 0
    planeAxisX_ = points_[1] - points_[0]; //x axis can be any vector on plane
    planeAxisX_.normalize();
    planeAxisY_ = Vector3D::crossProduct(planeAxisX_, planeNormal); //y axis will be any vector orthogonal to both x axis and planeNormal
    planeAxisY_.normalize();
    
#ifdef DEBUG_MODE
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, planeAxisX_), 0)) {
        writeLog(ERROR, "dot product between plane normal of polygon and chosen x-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, planeAxisX_)));
    }
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, planeAxisY_), 0)) {
        writeLog(ERROR, "dot product between plane normal of polygon and chosen y-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, planeAxisY_)));
    }
    if (!doubleEquals(Vector3D::dotProduct(planeAxisX_, planeAxisY_), 0)) {
        writeLog(ERROR, "dot product between chosen x-axis and y-axis of polygon plane = %f does not equal 0", fabs(Vector3D::dotProduct(planeAxisX_, planeAxisY_)));
    }
#endif
    
    //create clipper representation
    for (vector<Vector3D>::const_reverse_iterator rit = points_.rbegin(); rit != points_.rend(); rit++) {
        Vector3D mappedPoint = mapPointToXYPlane(*rit);
        
        if ((fabs(mappedPoint.x()) >= pow(2, 62) / mappedPointPrecision_) || (fabs(mappedPoint.y()) >= pow(2, 62) / mappedPointPrecision_)) {
            writeLog(WARNING, "mapping point from polygon that excedes given range");
        }
        polygonXYPlane_ << ClipperLib::IntPoint((int)(mappedPoint.x() * mappedPointPrecision_), (int)(mappedPoint.y() * mappedPointPrecision_));
    }
}

const vector<Vector3D> & Polygon::points() const {
    return points_;
}

const Plane & Polygon::plane() const {
    return plane_;
}

Polygon Polygon::mapToXYPlane() const {
    vector<Vector3D> mappedPoints;
    for (vector<Vector3D>::const_iterator it = points_.begin(); it != points_.end(); it++) {
        mappedPoints.push_back(mapPointToXYPlane(*it));
    }
    return Polygon(mappedPoints);
}

Vector3D Polygon::mapPointToXYPlane(const Vector3D & point) const {
    //map to x/y plane using method described in stackoverflow post: http://stackoverflow.com/questions/23472048/projecting-3d-points-to-2d-plane#23474396
    double mappedX = Vector3D::dotProduct(planeAxisX_, point - plane_.origin());
    double mappedY = Vector3D::dotProduct(planeAxisY_, point - plane_.origin());
    return Vector3D(mappedX, mappedY, 0);
}

double Polygon::area() const {
    double clipperArea = ClipperLib::Area(polygonXYPlane_);
    double mappedPointPrecisionSquared = (double)(mappedPointPrecision_ * mappedPointPrecision_);
    return clipperArea / mappedPointPrecisionSquared;
}

bool Polygon::pointInPolygon(const Vector3D & point) const {
    Vector3D mappedPoint = mapPointToXYPlane(point);
    return ClipperLib::PointInPolygon(ClipperLib::IntPoint((int)(mappedPoint.x() * mappedPointPrecision_), (int)(mappedPoint.y() * mappedPointPrecision_)), polygonXYPlane_);
}

uint64_t Polygon::mappedPointPrecision() {
    return Polygon::mappedPointPrecision_;
}

void Polygon::mappedPointPrecision(int mappedPointPrecision) {
    Polygon::mappedPointPrecision_ = mappedPointPrecision;
}

string Polygon::toString() const {
    ostringstream stream;
    stream << "[";
    bool addComma = false;
    for (vector<Vector3D>::const_iterator it = points_.begin(); it != points_.end(); it++) {
        if (addComma) {
            stream << ", ";
        }
        stream << it->toString();
        addComma = true;
    }
    return stream.str();
}
