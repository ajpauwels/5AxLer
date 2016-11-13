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

BuildMap::BuildMap(std::shared_ptr<Mesh> p_mesh) :
m_p_mesh(p_mesh) { }

bool BuildMap::solve() {
    if (!m_solved) {
        //all statics - should only be executed once
        
        static vector<pair<int, int>> ellipseCoors; //ellipse to remove from build map with center of v.theta() and v.phi()
        static bool ellipseBuilt = false; //whether or not ellipse has been calculated
        
        if (!ellipseBuilt) {
            //use ceil() to over-estimate area
            double deltaTheta = thetaToBAxisRange(THETA_MAX);
            double deltaPhi = phiToAAxisRange(THETA_MAX);
            
            //to overapproximate ellipse, we extend the radius by this constant
            double radiusExtension = 1.0 / cos(M_PI / ELLIPSE_PRECISION);
            
            writeLog(INFO, "BUILD MAP - delta-theta: %f", deltaTheta);
            writeLog(INFO, "BUILD MAP - delta-phi: %f", deltaPhi);
            writeLog(INFO, "BUILD MAP - ellipse area: %f", M_PI * deltaTheta * deltaPhi);
            writeLog(INFO, "BUILD MAP - radius extension: %f", radiusExtension);
            
            //polygon goes in clockwise form
            for (unsigned int i = 0; i < ELLIPSE_PRECISION; i++) {
                double angle = 2.0 * M_PI * static_cast<double>(i) / static_cast<double>(ELLIPSE_PRECISION);
                
                double xDouble = deltaTheta * radiusExtension * cos(angle);
                double yDouble = deltaPhi * radiusExtension * sin(angle);
                
                int xInt = (xDouble > 0) ? ceil(xDouble) : floor(xDouble);
                int yInt = (yDouble > 0) ? ceil(yDouble) : floor(yDouble);
                
                ellipseCoors.push_back(pair<int, int>(xInt, yInt));
            }
            
            ellipseBuilt = true;
        }
        
        //end static section
        
        //remove all contraints from face normals
        Paths holes;
        for (vector<shared_ptr<Mesh::Face>>::const_iterator it = m_p_mesh->p_faces().begin(); it != m_p_mesh->p_faces().end(); it++) {
            Vector3D v = (*it)->normal();
            v = v * -1;
            
            printf("v theta: %f phi: %f\n", v.theta().val(), v.phi().val());
            
            if (v.phi().val() == 0) {
                m_phiZeroAvailable = false;
                
                Path hole;
                hole << IntPoint(0, 0) << IntPoint(0, phiToAAxisRange(THETA_MAX)) << IntPoint(B_AXIS_DISCRETE_POINTS, phiToAAxisRange(THETA_MAX)) << IntPoint(B_AXIS_DISCRETE_POINTS, 0);
                
                //union all holes into one polygon
                Clipper holeClipper;
                holeClipper.AddPaths(holes, ptSubject, true);
                holeClipper.AddPath(hole, ptClip, true);
                if (!holeClipper.Execute(ctUnion, holes, pftNonZero, pftNonZero)) {
                    writeLog(ERROR, "BUILD MAP - error taking union of holes");
                    return false;
                }
            } else {
                //if top point of build map is covered, set phiZeroAvailable to false
                m_phiZeroAvailable &= (fabs(v.phi().val()) > THETA_MAX);
                
                int xCenter = thetaToBAxisRange(v.theta());
                int yCenter = phiToAAxisRange(v.phi());
                
                double sinPhi = v.phi().sinVal();
                
                Path hole,
                holeWrapThetaPos,
                holeWrapThetaNeg;
                
                bool wrapAroundThetaPos = false,
                wrapAroundThetaNeg = false;
                
                for (vector<pair<int, int>>::iterator it = ellipseCoors.begin(); it < ellipseCoors.end(); it++) {
                    int x = (it->first / sinPhi) + xCenter;
                    int y = it->second + yCenter;
                    hole << IntPoint(x, y);
                    
                    if (!wrapAroundThetaPos) {
                        if (x > B_AXIS_DISCRETE_POINTS) {
                            wrapAroundThetaPos = true;
                        }
                    }
                    holeWrapThetaPos << IntPoint(x - B_AXIS_DISCRETE_POINTS, y);
                    
                    if (!wrapAroundThetaNeg) {
                        if (x < 0) {
                            wrapAroundThetaNeg = true;
                        }
                    }
                    holeWrapThetaNeg << IntPoint(x + B_AXIS_DISCRETE_POINTS, y);
                }
                
                //union all holes into one polygon
                Clipper holeClipper;
                holeClipper.AddPaths(holes, ptSubject, true);
                
                holeClipper.AddPath(hole, ptClip, true);
                if (wrapAroundThetaPos) {
                    holeClipper.AddPath(holeWrapThetaPos, ptClip, true);
                }
                if (wrapAroundThetaNeg) {
                    holeClipper.AddPath(holeWrapThetaNeg, ptClip, true);
                }
                
                if (!holeClipper.Execute(ctUnion, holes, pftNonZero, pftNonZero)) {
                    writeLog(ERROR, "BUILD MAP - error taking union of holes");
                    return false;
                }
            }
            
            //TODO will this be more efficient?
            //TODO check area of holes and if it's >= total build map area then we know the build map is empty and we can stop here
            
            //SimplifyPolygons(holes_); //I don't think this is necessary and takes extra time but not sure
        }
        
        Clipper buildMapClipper;
        //set up subject (only look in this box)
        Path subject;
        subject << IntPoint(0, 0) << IntPoint(0, A_AXIS_DISCRETE_POINTS) << IntPoint(B_AXIS_DISCRETE_POINTS, A_AXIS_DISCRETE_POINTS) << IntPoint(B_AXIS_DISCRETE_POINTS, 0);
        buildMapClipper.AddPath(subject, ptSubject, true);
        buildMapClipper.AddPaths(holes, ptClip, true);
        if (!buildMapClipper.Execute(ctDifference, m_buildMap2D, pftNonZero, pftNonZero)) {
            writeLog(ERROR, "BUILD MAP - error taking difference of map and holes");
            return false;
        }
        m_solved = true;
    }
    
    return true;
}

double BuildMap::area() const {
    if (!m_solved) {
        writeLog(WARNING, "BUILD MAP - taking area of unsolved build map");
        return 0;
    } else if (m_buildMap2D.size() == 0) { //check to make sure paths are not empty (meaning empty area)
        return 0;
    }
    
    double area = 0;
    for (unsigned int i = 0; i < m_buildMap2D.size(); i++) {
        area += Area(m_buildMap2D[i]);
    }
    return area;
}

bool BuildMap::checkVector(const Vector3D & v, bool includeEdges) const {
    if (!m_solved) {
        writeLog(WARNING, "BUILD MAP - checking vector of unsolved build map");
        return false;
    } else if (area() == 0) { //build map is empty
        return false;
    } else if (v.phi().val() == 0) {
        return m_phiZeroAvailable;
    }
    
    //will return 0 if false, -1 if on edge, 1 otherwise
    int pointIn = PointInPolygon(IntPoint(thetaToBAxisRange(v.theta()), phiToAAxisRange(v.phi())), m_buildMap2D[0]);
    return (includeEdges ? (pointIn != 0) : (pointIn == 1));
}

Vector3D BuildMap::findValidVectorRecursive(int xStart, int yStart, int width, int height) const {
#ifdef DEBUG_MODE
    //writeLog(INFO, "BUILD MAP - checking build map theta(%d-%d) phi(%d-%d)", xStart, xStart + width, yStart, yStart + height);
#endif
    if ((width == 1) && (height == 1)) {
        return Vector3D(BuildMap::bAxisValToTheta(xStart), BuildMap::aAxisValToPhi(yStart));
    }
    
    Clipper searchClipper;
    searchClipper.AddPaths(m_buildMap2D, ptSubject, true);
    
    bool cutHorizontally = width < height;
    int dx = (cutHorizontally ? width : ceil(static_cast<double>(width) / 2.0));
    int dy = (cutHorizontally ? ceil(static_cast<double>(height) / 2.0) : height);
    
    Path search;
    search << IntPoint(xStart, yStart) << IntPoint(xStart, yStart + dy) << IntPoint(xStart + dx, yStart + dy) << IntPoint(xStart + dx, yStart);
    searchClipper.AddPath(search, ptClip, true);
    
    Paths solution;
    searchClipper.Execute(ctIntersection, solution, pftNonZero, pftNonZero);
    
    bool searchSuccess;
    if (solution.size() != 0) {
        double area;
        for (unsigned int i = 0; i < solution.size(); i++) {
            area += Area(solution[i]);
        }
#ifdef DEBUG_MODE
        //writeLog(INFO, "BUILD MAP - area of region: %f", area);
#endif
        searchSuccess = (area > 0);
    } else {
        searchSuccess = false;
    }
    
    return findValidVectorRecursive(xStart + width - dx, yStart + height - dy, dx, dy);
}

Vector3D BuildMap::findValidVector() const {
    if (!m_solved) {
        writeLog(WARNING, "BUILD MAP - finding valid vector of unsolved build map");
        return Vector3D(0, 0, 0);
    } else if (area() == 0) {
        return Vector3D(0, 0, 0);
    }
    
    Vector3D v = findValidVectorRecursive(0, 0, B_AXIS_DISCRETE_POINTS, A_AXIS_DISCRETE_POINTS);
    if (!checkVector(v)) {
        writeLog(ERROR, "BUILD MAP - arbitrary vector(%f, %f, %f) not in buildmap", v.x(), v.y(), v.z());
    }
    return v;
}

pair<Vector3D, double> BuildMap::findBestVectorRecursive(int x, int y, int dx, int dy, double prevHeuristic) const {
    vector<pair<Vector3D, double>> options; //Vector3D with lowest heuristic in this vector is the best vector
    
    double north = averageCuspHeight(Vector3D(bAxisValToTheta((x + dx) % B_AXIS_DISCRETE_POINTS), aAxisValToPhi(y)));
    double south = averageCuspHeight(Vector3D(bAxisValToTheta((x - dx) % B_AXIS_DISCRETE_POINTS), aAxisValToPhi(y)));
    double east = averageCuspHeight(Vector3D(bAxisValToTheta(x), aAxisValToPhi((y + dy) % A_AXIS_DISCRETE_POINTS)));
    double west = averageCuspHeight(Vector3D(bAxisValToTheta(x), aAxisValToPhi((y - dy) % A_AXIS_DISCRETE_POINTS)));
    
    int newDx = ceil(static_cast<double>(dx) / 2.0);
    int newDy = ceil(static_cast<double>(dy) / 2.0);
    
    if (prevHeuristic < fmin(north, fmin(south, fmin(east, west)))) { //there is no better option so continue narrowing down search
        if ((dx == 1) && (dy == 1)) {
            options.push_back(pair<Vector3D, double>(Vector3D(bAxisValToTheta(x), aAxisValToPhi(y)), prevHeuristic));
        } else {
            options.push_back(findBestVectorRecursive(x, y, newDx, newDy, prevHeuristic));
        }
    } else {
        if (north < prevHeuristic) {
            options.push_back(findBestVectorRecursive((x + newDx) % B_AXIS_DISCRETE_POINTS, y, newDx, newDy, north));
        }
        if (south < prevHeuristic) {
            options.push_back(findBestVectorRecursive((x - newDx) % B_AXIS_DISCRETE_POINTS, y, newDx, newDy, south));
        }
        if (east < prevHeuristic) {
            options.push_back(findBestVectorRecursive(x, (y + newDy) % A_AXIS_DISCRETE_POINTS, newDx, newDy, east));
        }
        if (west < prevHeuristic) {
            options.push_back(findBestVectorRecursive(x, (y + newDy) % A_AXIS_DISCRETE_POINTS, newDx, newDy, west));
        }
    }
    
    pair<Vector3D, double> bestOption(Vector3D(0, 0, 0), INFINITY);
    for (vector<pair<Vector3D, double>>::iterator it = options.begin(); it != options.end(); it++) {
        if (it->second < bestOption.second) {
            bestOption = *it;
        }
    }
    
    if (bestOption.first == Vector3D(0, 0, 0)) {
        writeLog(ERROR, "BUILD MAP - finding best vector returned no valid options");
    }
    
    return bestOption;
}

Vector3D BuildMap::findBestVector() const {
    if (!m_solved) {
        writeLog(WARNING, "BUILD MAP - finding best vector of unsolved build map");
        return Vector3D(0, 0, 0);
    }
    
    Vector3D v = findValidVector();
    double heuristic = averageCuspHeight(v);
    
    return findBestVectorRecursive(thetaToBAxisRange(v.theta()), phiToAAxisRange(v.phi()), B_AXIS_DISCRETE_POINTS / 2, A_AXIS_DISCRETE_POINTS / 2, heuristic).first;
}

double BuildMap::averageCuspHeight(const Vector3D & v) const {
    if (!m_solved) {
        writeLog(WARNING, "BUILD MAP - weighing vector of unsolved build map");
        return INFINITY;
    } else if (!checkVector(v)) {
        return INFINITY;
    }
    
    double weight = 0;
    double totalFaceArea;
    for (vector<shared_ptr<Mesh::Face>>::const_iterator it = m_p_mesh->p_faces().begin(); it != m_p_mesh->p_faces().end(); it++) {
        shared_ptr<Mesh::Face> p_face = *it;
        
        double weightToAdd = Vector3D::dotProduct(v, p_face->normal()) * p_face->area();
        weightToAdd /= (v.magnitude() * p_face->normal().magnitude());
        weight += fabs(weightToAdd);
        
        totalFaceArea += p_face->area();
    }
    weight *= SLICE_THICKNESS;
    weight /= totalFaceArea;
    return weight;
}

Vector3D BuildMap::mapToVector(int x, int y) {
    return Vector3D(bAxisValToTheta(x), aAxisValToPhi(y));
}

std::pair<int, int> BuildMap::vector3DToMap(const Vector3D & v) {
    return pair<int, int>(thetaToBAxisRange(v.theta()), phiToAAxisRange(v.phi()));
}

int BuildMap::thetaToBAxisRange(const Angle & theta) {
    return Angle::radiansToDegrees(theta.val()) / B_AXIS_PRECISION_DEGREES;
}

int BuildMap::phiToAAxisRange(const Angle & phi) {
    return Angle::radiansToDegrees(phi.val()) / A_AXIS_PRECISION_DEGREES;
}

Angle BuildMap::bAxisValToTheta(double bAxisVal) {
    return Angle(Angle::degreesToRadians(bAxisVal * B_AXIS_PRECISION_DEGREES));
}

Angle BuildMap::aAxisValToPhi(double aAxisVal) {
    return Angle(Angle::degreesToRadians(aAxisVal * A_AXIS_PRECISION_DEGREES));
}
