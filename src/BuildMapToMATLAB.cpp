//
//  BuildMapToMATLAB.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/9/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "BuildMapToMATLAB.hpp"

#include <fstream>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

bool BuildMapToMATLAB::parseBuildMapToMATLAB(string filePath, const BuildMap & buildMap, OutputType type, int precision) {
    double oldPrecision = precision;
    precision = fmax(1, fmin(precision, fmin(A_AXIS_DISCRETE_POINTS, B_AXIS_DISCRETE_POINTS)));
    
    if (precision != oldPrecision) {
        writeLog(WARNING, "precision of BuildMap is out of range");
    }
    
    ofstream file;
    file.open(filePath, ios::out | ios::binary);
    
    if (file.is_open()) {
        ostringstream xStr, yStr, zStr;
        for (int y = 0; y <= A_AXIS_DISCRETE_POINTS; y += precision) {
            for (int x = 0; x <= B_AXIS_DISCRETE_POINTS; x += precision) {
                Vector3D v = BuildMap::mapToVector(x, y);
                double weight = buildMap.checkVector(v) ? buildMap.averageCuspHeight(v) : -1;
                
                if (type == PLANE) {
                    xStr << x << " ";
                    yStr << y << " ";
                    zStr << (int)weight << " ";
                } else if (type == SPHERE) {
                    weight = fmax(weight, 0);
                    v.normalize(weight);
                    xStr << v.x() << " ";
                    yStr << v.y() << " ";
                    zStr << v.z() << " ";
                } else if (type == SPHERE_SMOOTH) {
                    weight = (weight > 0) ? 1 : 0;
                    v.normalize(weight);
                    xStr << v.x() << " ";
                    yStr << v.y() << " ";
                    zStr << v.z() << " ";
                }
            }
            
            xStr << ";\n";
            yStr << ";\n";
            zStr << ";\n";
        }
        
        file << "X = [\n" << xStr.str() << "];\n";
        file << "Y = [\n" << yStr.str() << "];\n";
        file << "Z = [\n" << zStr.str() << "];\n";
        file << "figure\n";
        file << "surf(X, Y, Z)";
        file.close();
        
        return true;
    } else {
        writeLog(WARNING, "could not open MATLAB script file");
        return false;
    }
}
