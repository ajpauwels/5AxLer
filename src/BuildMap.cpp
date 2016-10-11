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
using namespace ClipperLib;

BuildMap::BuildMap() { }

bool BuildMap::checkOrientation(Angle theta, Angle phi, bool includeEdges) {
    if (!solved_) {
        Clipper buildMapClipper;
        //set up subject (only look in this box)
        Path subject;
        subject << IntPoint(0, 0) << IntPoint(0, A_AXIS_RANGE) << IntPoint(B_AXIS_RANGE, A_AXIS_RANGE) << IntPoint(B_AXIS_RANGE, 0);
        buildMapClipper.AddPath(subject, ptSubject, true);
        buildMapClipper.AddPaths(holes_, ptClip, true);
        buildMapClipper.Execute(ctDifference, buildMap2D_, pftNonZero, pftNonZero);
        solved_ = true;
    }
    
    if (buildMap2D_.size() == 0) { //build map is empty
        return false;
    }
    
    //will return 0 if false, -1 if on edge, 1 otherwise
    int pointIn = PointInPolygon(IntPoint(thetaToBAxisRange(theta), phiToAAxisRange(phi)), buildMap2D_[0]);
    return (includeEdges ? (pointIn != 0) : (pointIn == 1));
}

void BuildMap::addConstraintVector(Vector3D v) {
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
        for (int i = 0; i < ELLIPSE_PRECISION; i++) {
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
    
#ifdef DEBUG_MODE
    //writeLog(INFO_MESSAGE, "removing all unit vectors v with dotProduct(v, vector(theta: %f, phi: %f)) >= cos(%f) = %f", v.theta().val(), v.phi().val(), THETA_MAX, cos(THETA_MAX));
#endif
    
    int xCenter = thetaToBAxisRange(v.theta());
    int yCenter = phiToAAxisRange(v.phi());
    
    Path hole;
    for (vector<pair<int, int>>::iterator it = ellipseCoors.begin(); it < ellipseCoors.end(); it++) {
        hole << IntPoint(fmin(B_AXIS_RANGE, fmax(0, it->first + xCenter)), fmin(A_AXIS_RANGE, fmax(0, it->second + yCenter)));
    }
    
    //union all holes into one polygon
    Clipper holeClipper;
    holeClipper.AddPaths(holes_, ptSubject, true);
    holeClipper.AddPath(hole, ptClip, true);
    holeClipper.Execute(ctUnion, holes_, pftNonZero, pftNonZero);
    //SimplifyPolygons(holes_); //I don't think this is necessary and takes extra time but not sure
    
    solved_ = false;
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
