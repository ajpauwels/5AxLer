//
//  BuildMap.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "BuildMap.hpp"

#include <cmath>
#include <vector>

#define ELLIPSE_PRECISION 100

using namespace mapmqp;
using namespace std;

BuildMap::BuildMap(Vector3D faceNormals[], double faceAreas[], int faceCount) :
faceNormals_(faceNormals),
faceAreas_(faceAreas),
faceCount_(faceCount) { }

bool BuildMap::solve() {
    if (!solved_) {
        //all statics - should only be executed once
        
        static vector<pair<int, int>> ellipseCoors; //ellipse to remove from build map with center of v.theta() and v.phi()
        static bool ellipseBuilt = false; //whether or not ellipse has been calculated
        
        if (!ellipseBuilt) {
            //use ceil() to over-estimate area
            double deltaTheta = ceil(((M_PI - THETA_MAX) * B_AXIS_RANGE) / (2 * M_PI));
            double deltaPhi = ceil(((M_PI - THETA_MAX) * A_AXIS_RANGE * 2) / (2 * M_PI));
            
            writeLog(INFO_MESSAGE, "BUILD MAP - delta-theta: %f", deltaTheta);
            writeLog(INFO_MESSAGE, "BUILD MAP - delta-phi: %f", deltaPhi);
            writeLog(INFO_MESSAGE, "BUILD MAP - ellipse area: %f", M_PI * deltaTheta * deltaPhi);
            
            //polygon goes in clockwise form
            for (unsigned int i = 0; i < ELLIPSE_PRECISION; i++) {
                double angle = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(ELLIPSE_PRECISION);
                
                //TODO this underapproximates ellipse, it should overapproximate to guarantee no false-negatives
                double xDouble = deltaTheta * cos(angle);
                double yDouble = deltaPhi * sin(angle);
                
                int xInt = (xDouble > 0) ? (ceil(xDouble) + 1) : (floor(xDouble) - 1);
                int yInt = (yDouble > 0) ? (ceil(yDouble) + 1) : (floor(yDouble) - 1);
                
                ellipseCoors.push_back(pair<int, int>(xInt, yInt));
            }
            
            ellipseBuilt = true;
        }
        
        //end static section
        
        //remove all contraints from face normals
        ClipperLib::Paths holes;
        for (unsigned int i = 0; i < faceCount_; i++) {
            Vector3D v = faceNormals_[i];
            
//#ifdef DEBUG_MODE
            //writeLog(INFO_MESSAGE, "removing all unit vectors v with dotProduct(v, vector(theta: %f, phi: %f)) >= cos(%f) = %f", v.theta().val(), v.phi().val(), THETA_MAX, cos(THETA_MAX));
//#endif
            
            int xCenter = thetaToBAxisRange(v.theta());
            int yCenter = phiToAAxisRange(v.phi());
            
            ClipperLib::Path hole;
            for (vector<pair<int, int>>::iterator it = ellipseCoors.begin(); it < ellipseCoors.end(); it++) {
                hole << ClipperLib::IntPoint(fmin(B_AXIS_RANGE, fmax(0, it->first + xCenter)), fmin(A_AXIS_RANGE, fmax(0, it->second + yCenter)));
            }
            
            //union all holes into one polygon
            ClipperLib::Clipper holeClipper;
            holeClipper.AddPaths(holes, ClipperLib::ptSubject, true);
            holeClipper.AddPath(hole, ClipperLib::ptClip, true);
            if (!holeClipper.Execute(ClipperLib::ctUnion, holes, ClipperLib::pftNonZero, ClipperLib::pftNonZero)) {
                writeLog(ERROR_MESSAGE, "BUILD MAP - error taking union of holes");
                return false;
            }
            
            //TODO check area of holes and if it's >= total build map area then we know the build map is empty and we can stop here
            
            //SimplifyPolygons(holes_); //I don't think this is necessary and takes extra time but not sure
        }
        
        ClipperLib::Clipper buildMapClipper;
        //set up subject (only look in this box)
        ClipperLib::Path subject;
        subject << ClipperLib::IntPoint(0, 0) << ClipperLib::IntPoint(0, A_AXIS_RANGE) << ClipperLib::IntPoint(B_AXIS_RANGE, A_AXIS_RANGE) << ClipperLib::IntPoint(B_AXIS_RANGE, 0);
        buildMapClipper.AddPath(subject, ClipperLib::ptSubject, true);
        buildMapClipper.AddPaths(holes, ClipperLib::ptClip, true);
        if (!buildMapClipper.Execute(ClipperLib::ctDifference, buildMap2D_, ClipperLib::pftNonZero, ClipperLib::pftNonZero)) {
            writeLog(ERROR_MESSAGE, "BUILD MAP - error taking difference of map and holes");
            return false;
        }
        solved_ = true;
    }
    
    return true;
}

double BuildMap::area() const {
    if (!solved_) {
        writeLog(WARNING_MESSAGE, "BUILD MAP - taking area of unsolved build map");
        return 0;
    }
    
    //check to make sure paths are not empty (meaning empty area)
    if (buildMap2D_.size() == 0) {
        return 0;
    }
    
    double area = 0;
    for (unsigned int i = 0; i < buildMap2D_.size(); i++) {
        area += ClipperLib::Area(buildMap2D_[i]);
    }
    return area;
}

bool BuildMap::checkVector(Vector3D v, bool includeEdges) const {
    if (!solved_) {
        writeLog(WARNING_MESSAGE, "BUILD MAP - checking vector of unsolved build map");
        return false;
    }
    
    if (area() == 0) { //build map is empty
        return false;
    }
    
    //will return 0 if false, -1 if on edge, 1 otherwise
    int pointIn = PointInPolygon(ClipperLib::IntPoint(thetaToBAxisRange(v.theta()), phiToAAxisRange(v.phi())), buildMap2D_[0]);
    return (includeEdges ? (pointIn != 0) : (pointIn == 1));
}

Vector3D BuildMap::findValidVectorRecursive(int xStart, int yStart, int width, int height) const {
#ifdef DEBUG_MODE
    //writeLog(INFO_MESSAGE, "BUILD MAP - checking build map theta(%d-%d) phi(%d-%d)", xStart, xStart + width, yStart, yStart + height);
#endif
    if ((width == 1) && (height == 1)) {
        return Vector3D(BuildMap::bAxisValToTheta(xStart), BuildMap::aAxisValToPhi(yStart));
    }
    
    ClipperLib::Clipper searchClipper;
    searchClipper.AddPaths(buildMap2D_, ClipperLib::ptSubject, true);
    
    bool cutHorizontally = width < height;
    int dx = (cutHorizontally ? width : ceil(static_cast<double>(width) / 2.0));
    int dy = (cutHorizontally ? ceil(static_cast<double>(height) / 2.0) : height);
    
    ClipperLib::Path search;
    search << ClipperLib::IntPoint(xStart, yStart) << ClipperLib::IntPoint(xStart, yStart + dy) << ClipperLib::IntPoint(xStart + dx, yStart + dy) << ClipperLib::IntPoint(xStart + dx, yStart);
    searchClipper.AddPath(search, ClipperLib::ptClip, true);
    
    ClipperLib::Paths solution;
    searchClipper.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
    
    bool searchSuccess;
    if (solution.size() != 0) {
        double area;
        for (unsigned int i = 0; i < solution.size(); i++) {
            area += Area(solution[i]);
        }
#ifdef DEBUG_MODE
        //writeLog(INFO_MESSAGE, "BUILD MAP - area of region: %f", area);
#endif
        searchSuccess = (area > 0);
    } else {
        searchSuccess = false;
    }
    
    return findValidVectorRecursive(xStart + width - dx, yStart + height - dy, dx, dy);
}

Vector3D BuildMap::findValidVector() const {
    if (!solved_) {
        writeLog(WARNING_MESSAGE, "BUILD MAP - finding valid vector of unsolved build map");
        return Vector3D(0, 0, 0);
    }
    
    if (area() == 0) {
        return Vector3D(0, 0, 0);
    }
    
    Vector3D v = findValidVectorRecursive(0, 0, B_AXIS_RANGE, A_AXIS_RANGE);
    if (!checkVector(v)) {
        writeLog(ERROR_MESSAGE, "BUILD MAP - arbitrary vector(%f, %f, %f) not in buildmap", v.x(), v.y(), v.z());
    }
    return v;
}

Vector3D BuildMap::findBestVectorRecursive(int x, int y, int dx, int dy, double heuristic) const {
    double north = weighVector(Vector3D(bAxisValToTheta((x + dx) % B_AXIS_RANGE), aAxisValToPhi(y)));
    double south = weighVector(Vector3D(bAxisValToTheta((x - dx) % B_AXIS_RANGE), aAxisValToPhi(y)));
    double east = weighVector(Vector3D(bAxisValToTheta(x), aAxisValToPhi((y + dy) % A_AXIS_RANGE)));
    double west = weighVector(Vector3D(bAxisValToTheta(x), aAxisValToPhi((y - dy) % A_AXIS_RANGE)));
    
    double maxHeuristic = fmin(heuristic, fmin(north, fmin(south, fmin(east, west))));
    
    int newDx = ceil(static_cast<double>(dx) / 2.0);
    int newDy = ceil(static_cast<double>(dy) / 2.0);
    
    if (maxHeuristic == heuristic) {
        //if we have reach minimum and search has narrowed down fully
        if ((dx == 1) && (dy == 1)) {
            return Vector3D(bAxisValToTheta(x), aAxisValToPhi(y));
        }
        
        return findBestVectorRecursive(x, y, newDx, newDy, heuristic);
    } else if (maxHeuristic == north) {
        return findBestVectorRecursive((x + newDx) % B_AXIS_RANGE, y, newDx, newDy, north);
    } else if (maxHeuristic == south) {
        return findBestVectorRecursive((x - newDx) % B_AXIS_RANGE, y, newDx, newDy, south);
    } else if (maxHeuristic == east) {
        return findBestVectorRecursive(x, (y + newDy) % A_AXIS_RANGE, newDx, newDy, east);
    } else { //if (maxHeuristic == west)
        return findBestVectorRecursive(x, (y + newDy) % A_AXIS_RANGE, newDx, newDy, west);
    }
}

Vector3D BuildMap::findBestVector() const {
    if (!solved_) {
        writeLog(WARNING_MESSAGE, "BUILD MAP - finding best vector of unsolved build map");
        return Vector3D(0, 0, 0);
    }
    
    Vector3D v = findValidVector();
    double heuristic = weighVector(v);
    
    return findBestVectorRecursive(thetaToBAxisRange(v.theta()), phiToAAxisRange(v.phi()), B_AXIS_RANGE / 2, A_AXIS_RANGE / 2, heuristic);
}

double BuildMap::weighVector(Vector3D v) const {
    if (!solved_) {
        writeLog(WARNING_MESSAGE, "BUILD MAP - weighing vector of unsolved build map");
        return INFINITY;
    }
    
    if (!checkVector(v)) {
        return INFINITY;
    }
    
    //uncomment lines of code to measure cusp height accurately instead of relatively
    
    double weight = 0;
    //double totalFaceArea;
    for (unsigned int i = 0; i < faceCount_; i++) {
#ifdef DEBUG_MODE
        if (Vector3D::dotProduct(v, faceNormals_[i]) > cos(THETA_MAX)) {
            writeLog(ERROR_MESSAGE, "BUILD MAP - dotProduct(v, faceNormals[%d]) = %d > cos(THETA_MAX) = %d", i, Vector3D::dotProduct(v, faceNormals_[i]), cos(THETA_MAX));
        }
#endif
        weight += Vector3D::dotProduct(v, faceNormals_[i]) * faceAreas_[i];
        //totalFaceArea += faceAreas_[i];
    }
    //weight *= SLICE_THICKNESS;
    //weight /= totalFaceArea;
    return weight;
}

int BuildMap::phiToAAxisRange(Angle phi) {
    //phi ranges [0, 2*pi)
    return (phi.val() * 2 * A_AXIS_RANGE) / (2.0 * M_PI);
}

int BuildMap::thetaToBAxisRange(Angle theta) {
    //theta ranges [0, 2*pi)
    return (theta.val() * B_AXIS_RANGE) / (2.0 * M_PI);
}

Angle BuildMap::aAxisValToPhi(double aAxisVal) {
    //inverse of phiToAAxisRange(Angle phi)
    return Angle((2.0 * M_PI * aAxisVal) / (2 * A_AXIS_RANGE));
}

Angle BuildMap::bAxisValToTheta(double bAxisVal) {
    //inverse of thetaToBAxisRange(Angle theta)
    return Angle(((2.0 * M_PI * bAxisVal) / B_AXIS_RANGE));
}
