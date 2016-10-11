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
#include <cmath>
#include "Clock.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    srand(1);
    
    //for testing...
    mapmqp::BuildMap map;
    
#define THETA_POLE M_PI
#define PHI_POLE (M_PI / 2)
    
    mapmqp::Clock timer;
    
    for (int i = 0; i < 300000; i++) {
        if (i % 1000 == 0) {
            printf("i: %d\n", i);
        }
        
        double divergence = M_PI_2;
        //randomly divert from pole with range of (-divergence, divergence) radians in either direction
        mapmqp::Angle theta(THETA_POLE + ((((rand() % 2) == 0) ? 1 : -1) * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/divergence))));
        mapmqp::Angle phi(PHI_POLE + ((((rand() % 2) == 0) ? 1 : -1) * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/divergence))));
        map.addConstraintVector(mapmqp::Vector3D(theta, phi));
    }
    printf("all vectors removed: %ldms\n", timer.delta());
    
    map.checkOrientation(mapmqp::Angle(0), mapmqp::Angle(0));
    
    double originalArea = A_AXIS_RANGE * B_AXIS_RANGE;
    printf("original area: %f\n", originalArea);
    
    double area = 0;
    if (map.buildMap2D_.size() != 0) {
        for (int i = 0; i < map.buildMap2D_.size(); i++) {
            double tempArea = ClipperLib::Area(map.buildMap2D_[i]);
            area += tempArea;
        }
        
    }
    printf("area: %f\n", area);
    printf("area difference: %f\n", area - originalArea);
    
    return 0;
}
