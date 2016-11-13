//
//  main.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#define RUN_TESTS

#ifdef RUN_TESTS
#define CATCH_CONFIG_MAIN
#include "../libs/Catch/catch.hpp"
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//for debugging
//TODO remove these
#include "../libs/clipper/clipper.hpp"
#include "Mesh.hpp"
#include "Utility.hpp"
#include "Clock.hpp"
#include "BuildMap.hpp"
#include "ProcessSTL.hpp"
#include <cmath>
#include <unordered_map>
#include <string>
#include <memory>
#include "BuildMapToMATLAB.hpp"
#include "Slicer.hpp"
//end debugging

using namespace mapmqp;
using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    // #ifdef RUN_TESTS
    //     //run tests
    //     //mapmqp::writeLog(mapmqp::INFO, "running tests...");
    //     int catch_argc = argc;
    //     char ** catch_argvs;
    //     char * catch_a = new char[argc];
    //     catch_argvs = &catch_a;
    //     catch_main(catch_argc, catch_argvs);
    //     //return 0;
    // #endif
    
    //     mapmqp::writeLog(mapmqp::INFO, "starting 5AxLer at time %s", mapmqp::Clock::wallTimeString().c_str());
    //     mapmqp::settingsDocument();
    
    //     mapmqp::Mesh mesh;
    
    // Vector3D v1(Vector3D(0, 0, 0));
    // Vector3D v2(Vector3D(10, 10, 10));
    // Vector3D v3(Vector3D(-10, -10, -10));
    // Vector3D v4(Vector3D(2, 4, -10));
    // Vector3D v5(Vector3D(2, 4, -10));
    
    // writeLog(INFO, "v4 == v4 : %d", v4 == v5);
    
    /* testing ProcessSTL */
    
    std::shared_ptr<mapmqp::Mesh> p_mesh = ProcessSTL::constructMeshFromSTL(argv[1]);
    const std::vector<std::shared_ptr<mapmqp::Mesh::Face>> meshFaces = p_mesh->p_faces();
    for (uint32_t i = 0; i < meshFaces.size(); ++i) {
        const std::shared_ptr<mapmqp::Mesh::Face> currFace = meshFaces[i];
        
        printf("Face vertices are:\n");
        for (uint16_t j = 0; j < 3; ++j) {
            printf("\t[%f, %f, %f]", currFace->p_vertex(j)->vertex().x(), currFace->p_vertex(j)->vertex().y(), currFace->p_vertex(j)->vertex().z());
        }
        printf("\n");
        printf("Connected faces are:\n");
        for (uint16_t j = 0; j < 3; ++j) {
            printf("%d.)", j);
            const std::shared_ptr<const mapmqp::Mesh::Face> connFace = currFace->p_connectedFace(j);
            for (uint16_t k = 0; k < 3; ++k) {
                printf("\t[%f, %f, %f]", connFace->p_vertex(k)->vertex().x(), connFace->p_vertex(k)->vertex().y(), connFace->p_vertex(k)->vertex().z());
            }
            printf("\n");
        }
        printf("\n");
        
    }
    
    ProcessSTL::constructSTLfromMesh(*p_mesh, "debug/TestSTL.STL");
    
    mapmqp::BuildMap map(p_mesh);
    map.solve();
    BuildMapToMATLAB::parseBuildMapToMATLAB("debug/buildmap-plane.m", map, BuildMapToMATLAB::PLANE, 25);
    BuildMapToMATLAB::parseBuildMapToMATLAB("debug/buildmap-sphere.m", map, BuildMapToMATLAB::SPHERE, 25);
    BuildMapToMATLAB::parseBuildMapToMATLAB("debug/buildmap-sphere-smooth.m", map, BuildMapToMATLAB::SPHERE_SMOOTH, 25);
    
    /*map.checkVector(mapmqp::Vector3D(mapmqp::Angle(0), mapmqp::Angle(0)));
     
     double originalArea = A_AXIS_RANGE * B_AXIS_RANGE;
     printf("original area: %f\n", originalArea);
     
     double area = map.area();
     printf("area: %f\n", area);
     printf("area difference: %f\n", area - originalArea);
     
     mapmqp::Vector3D validVector = map.findValidVector();
     printf("valid vector(theta:%f, phi:%f)\n", validVector.theta().val(), validVector.phi().val());
     
     //TODO check to make sure this value is correct
     mapmqp::Vector3D bestVector = map.findBestVector();
     printf("best vector(theta:%f, phi:%f)\n", bestVector.theta().val(), bestVector.phi().val());*/
    
    return 0;
}
