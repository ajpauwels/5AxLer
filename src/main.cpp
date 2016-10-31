//
//  main.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include <iostream>
#include <stdlib.h>

#include "BuildMap.hpp"
#include "Clock.hpp"
#include <cmath>
#include <unordered_map>
#include <string>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    /*
    //testing build map
    srand(16);
    mapmqp::Clock timer;
    
#define THETA_POLE M_PI
#define PHI_POLE (M_PI / 2)
#define VECTOR_DIVERGENCE M_PI_2
#define MAX_AREA 40.0
    
#define n 200000
    
    mapmqp::Vector3D faceNormals[n];
    double faceAreas[n];
    
    for (int i = 0; i < n; i++) {
        if (i % 1000 == 0) {
            printf("i: %d\n", i);
        }
        
        //randomly divert from pole with range of (-divergence, divergence) radians in either direction
        mapmqp::Angle theta(THETA_POLE + ((((rand() % 2) == 0) ? 1 : -1) * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/VECTOR_DIVERGENCE))));
        mapmqp::Angle phi(PHI_POLE + ((((rand() % 2) == 0) ? 1 : -1) * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/VECTOR_DIVERGENCE))));
        
        faceNormals[i] = mapmqp::Vector3D(theta, phi);
        
        faceAreas[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/MAX_AREA);
        
        //map.addConstraintVector(mapmqp::Vector3D(theta, phi));
    }
    printf("random surface data generated: %ldms\n", timer.delta());
    
    mapmqp::BuildMap map(faceNormals, faceAreas, n);
    map.solve();
    
    printf("build map constructed: %ldms\n", timer.delta());
    
    map.checkVector(mapmqp::Vector3D(mapmqp::Angle(0), mapmqp::Angle(0)));
    
    double originalArea = A_AXIS_RANGE * B_AXIS_RANGE;
    printf("original area: %f\n", originalArea);
    
    double area = map.area();
    printf("area: %f\n", area);
    printf("area difference: %f\n", area - originalArea);
    
    mapmqp::Vector3D validVector = map.findValidVector();
    printf("valid vector(theta:%f, phi:%f)\n", validVector.theta().val(), validVector.phi().val());
    
    //TODO check to make sure this value is correct
    mapmqp::Vector3D bestVector = map.findBestVector();
    printf("best vector(theta:%f, phi:%f)\n", bestVector.theta().val(), bestVector.phi().val());
    */
     
    return 0;
}
