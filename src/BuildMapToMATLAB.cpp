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
    precision = fmax(1, fmin(precision, fmin(A_AXIS_RANGE, B_AXIS_RANGE)));
    
    if (precision != oldPrecision) {
        writeLog(WARNING, "precision of BuildMap is out of range");
    }
    
    ofstream file;
    file.open(filePath, ios::out | ios::binary);
    
    if (file.is_open()) {
        ostringstream xStr, yStr, zStr;
        for (int x = 0; x < B_AXIS_RANGE; x += precision) {
            for (int y = 0; y < A_AXIS_RANGE; y += precision) {
                Vector3D v(BuildMap::bAxisValToTheta(x), BuildMap::aAxisValToPhi(y));
                double weight = buildMap.weighVector(v).second ? buildMap.weighVector(v).first : 0;
                
                if (type == PLANE) {
                    xStr << x << " ";
                    yStr << y << " ";
                    zStr << (int)weight << " ";
                } else if (type == SPHERE) {
                    v.normalize(weight);
                    xStr << v.x() << " ";
                    yStr << v.y() << " ";
                    zStr << v.z() << " ";
                }
            }
            
            if (type == SPHERE) {
                for (int y = 0; y < A_AXIS_RANGE; y += precision) {
                        xStr << "0 ";
                        yStr << "0 ";
                        zStr << "0 ";
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
