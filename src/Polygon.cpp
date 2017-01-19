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

uint64_t Polygon::s_mappedPointPrecision = 1000000;

Polygon::Polygon(const vector<Vector3D> & points) :
m_points(points) {
    if (points.size() < 3) {
        writeLog(ERROR, "attempted to create Polygon with only 2 points");
        //TODO throw exception
        exit(0);
    }
    
    Vector3D p0, p1, p2;
    
    //find points to generate plane
    bool planeValid = false;
    p0 = points[0];
    for (vector<Vector3D>::const_iterator it = points.begin() + 1; it != points.end(); it++) {
        if (*it != p0) { //find point different than p0
            p1 = *it;
            for (vector<Vector3D>::const_iterator subIt = it + 1; subIt != points.end(); subIt++) {
                if (Vector3D::crossProduct(p1 - p0, *subIt - p0) != 0) { //find point on different line than p0 and p1
                    p2 = *subIt;
                    planeValid = true;
                    break;
                }
            }
            break;
        }
    }
    if (!planeValid) {
        writeLog(ERROR, "all points in Polygon lie on the same line");
        //TODO throw exception
        exit(0);
    }
    
    //set plane's normal
    Vector3D planeNormal = Vector3D::crossProduct(p1 - p0, p2 - p0);
    planeNormal.normalize();
    
    //find closest point on plane to origin
    double t = Vector3D::dotProduct(planeNormal, m_points[0]) / Vector3D::dotProduct(planeNormal, planeNormal);
    
    m_plane = Plane(planeNormal, t);
    
    //find x and y axes of polygon plane such that dot(x, planeNormal) = dot(y, planeNormal) = 0
    m_planeAxisX = Vector3D(planeNormal.theta(), planeNormal.phi() + M_PI_2);
    m_planeAxisY = Vector3D(planeNormal.theta() + M_PI_2, planeNormal.phi() + M_PI_2);
    m_planeAxisX.normalize();
    m_planeAxisY.normalize();
    
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, m_planeAxisX), 0.0)) {
        writeLog(ERROR, "dot product between plane normal of Polygon and chosen x-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, m_planeAxisX)));
    }
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, m_planeAxisY), 0.0)) {
        writeLog(ERROR, "dot product between plane normal of Polygon and chosen y-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, m_planeAxisY)));
    }
    if (!doubleEquals(Vector3D::dotProduct(m_planeAxisX, m_planeAxisY), 0.0)) {
        writeLog(ERROR, "dot product between chosen x-axis and y-axis of Polygon plane = %f does not equal 0", fabs(Vector3D::dotProduct(m_planeAxisX, m_planeAxisY)));
    }
    
    //create clipper representation
    for (vector<Vector3D>::const_reverse_iterator rit = m_points.rbegin(); rit != m_points.rend(); rit++) {
        Vector3D mappedPoint = mapPointToXYPlane(*rit);
        
        if ((fabs(mappedPoint.x()) >= pow(2, 62) / s_mappedPointPrecision) || (fabs(mappedPoint.y()) >= pow(2, 62) / s_mappedPointPrecision)) {
            writeLog(WARNING, "mapping point from Polygon that excedes given range");
        }
        m_polygonXYPlane << ClipperLib::IntPoint((int)(mappedPoint.x() * s_mappedPointPrecision), (int)(mappedPoint.y() * s_mappedPointPrecision));
    }
}

const vector<Vector3D> & Polygon::points() const {
    return m_points;
}

const Plane & Polygon::plane() const {
    return m_plane;
}

void Polygon::togglePolygonType() {
    ClipperLib::ReversePath(m_polygonXYPlane);
}

Polygon Polygon::mapToXYPlane() const {
    vector<Vector3D> mappedPoints;
    for (vector<Vector3D>::const_iterator it = m_points.begin(); it != m_points.end(); it++) {
        mappedPoints.push_back(mapPointToXYPlane(*it));
    }
    return Polygon(mappedPoints);
}

Vector3D Polygon::mapPointToXYPlane(const Vector3D & point) const {
    //map to x/y plane using method described in stackoverflow post: http://stackoverflow.com/questions/23472048/projecting-3d-points-to-2d-plane#23474396
    double mappedX = Vector3D::dotProduct(m_planeAxisX, point - m_plane.origin());
    double mappedY = Vector3D::dotProduct(m_planeAxisY, point - m_plane.origin());
    return Vector3D(mappedX, mappedY, 0);
}

Vector3D Polygon::mapPointToPolygonPlane(const Vector3D & point) const {
    if (point.z() != 0) {
        writeLog(WARNING, "mapping point to Polygon plane that has a non-zero z value");
    }
    
    Vector3D mappedX = m_planeAxisX;
    mappedX.normalize(point.x());
    Vector3D mappedY = m_planeAxisY;
    mappedY.normalize(point.y());
    return m_plane.origin() + mappedX + mappedY;
}

double Polygon::area() const {
    double clipperArea = ClipperLib::Area(m_polygonXYPlane);
    double mappedPointPrecisionSquared = (double)(s_mappedPointPrecision * s_mappedPointPrecision);
    return clipperArea / mappedPointPrecisionSquared;
}

bool Polygon::pointInPolygon(const Vector3D & point) const {
    Vector3D mappedPoint = mapPointToXYPlane(point);
    return ClipperLib::PointInPolygon(ClipperLib::IntPoint((int)(mappedPoint.x() * s_mappedPointPrecision), (int)(mappedPoint.y() * s_mappedPointPrecision)), m_polygonXYPlane);
}

uint64_t Polygon::mappedPointPrecision() {
    return Polygon::s_mappedPointPrecision;
}

void Polygon::mappedPointPrecision(int mappedPointPrecision) {
    Polygon::s_mappedPointPrecision = mappedPointPrecision;
}

string Polygon::toString() const {
    ostringstream stream;
    stream << "[";
    bool addComma = false;
    for (vector<Vector3D>::const_iterator it = m_points.begin(); it != m_points.end(); it++) {
        if (addComma) {
            stream << ", ";
        }
        stream << it->toString();
        addComma = true;
    }
    return stream.str();
}
