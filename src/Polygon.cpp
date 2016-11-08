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
        writeLog(ERROR, "attempted to create polygon with only 2 points");
        //TODO throw exception
    }
    
    //TODO first 3 points may be on same line, in which case plane normal cannot be determined by first 3 points
    //set plane's normal
    Vector3D planeNormal = Vector3D::crossProduct(points[1] - points[0], points[2] - points[0]); //TODO this is backwards?
    planeNormal.normalize();
    
    //find closest point on plane to origin
    double t = Vector3D::dotProduct(planeNormal, m_points[0]) / Vector3D::dotProduct(planeNormal, planeNormal);
    
    m_plane = Plane(planeNormal, t);
    
    //find arbitrary x and y axes of polygon plane such that dot(x, planeNormal) = dot(y, planeNormal) = 0
    m_planeAxisX = m_points[1] - m_points[0]; //x axis can be any vector on plane
    m_planeAxisX.normalize();
    m_planeAxisY = Vector3D::crossProduct(m_planeAxisX, planeNormal); //y axis will be any vector orthogonal to both x axis and planeNormal
    m_planeAxisY.normalize();
    
#ifdef DEBUG_MODE
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, m_planeAxisX), 0)) {
        writeLog(ERROR, "dot product between plane normal of polygon and chosen x-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, m_planeAxisX)));
    }
    if (!doubleEquals(Vector3D::dotProduct(planeNormal, m_planeAxisY), 0)) {
        writeLog(ERROR, "dot product between plane normal of polygon and chosen y-axis = %f does not equal 0", fabs(Vector3D::dotProduct(planeNormal, m_planeAxisY)));
    }
    if (!doubleEquals(Vector3D::dotProduct(m_planeAxisX, m_planeAxisY), 0)) {
        writeLog(ERROR, "dot product between chosen x-axis and y-axis of polygon plane = %f does not equal 0", fabs(Vector3D::dotProduct(m_planeAxisX, m_planeAxisY)));
    }
#endif
    
    //create clipper representation
    for (vector<Vector3D>::const_reverse_iterator rit = m_points.rbegin(); rit != m_points.rend(); rit++) {
        Vector3D mappedPoint = mapPointToXYPlane(*rit);
        
        if ((fabs(mappedPoint.x()) >= pow(2, 62) / s_mappedPointPrecision) || (fabs(mappedPoint.y()) >= pow(2, 62) / s_mappedPointPrecision)) {
            writeLog(WARNING, "mapping point from polygon that excedes given range");
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
