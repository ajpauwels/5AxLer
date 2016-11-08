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
#include "../libs/clipper/clipper.hpp"
#include "Mesh.hpp"
#include "Utility.hpp"
#include "Clock.hpp"
#include "BuildMap.hpp"
#include "ProcessSTL.hpp"
#include <cmath>
#include <unordered_map>
#include <string>
#include "Slicer.hpp"
//end debugging

using namespace mapmqp;

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
    
    mapmqp::ProcessSTL parser(argv[1]);
    std::shared_ptr<mapmqp::Mesh> p_mesh = parser.run();
    const std::vector<std::shared_ptr<mapmqp::MeshFace>> meshFaces = p_mesh->p_faces();
    for (uint32_t i = 0; i < meshFaces.size(); ++i) {
        const std::shared_ptr<mapmqp::MeshFace> currFace = meshFaces[i];
        
        printf("Face vertices are:\n");
        for (uint16_t j = 0; j < 3; ++j) {
            printf("\t[%f, %f, %f]", currFace->p_vertex(j)->vertex().x(), currFace->p_vertex(j)->vertex().y(), currFace->p_vertex(j)->vertex().z());
        }
        printf("\n");
        printf("Connected faces are:\n");
        for (uint16_t j = 0; j < 3; ++j) {
            printf("%d.)", j);
            const std::shared_ptr<const mapmqp::MeshFace> connFace = currFace->p_connectedFace(j);
            for (uint16_t k = 0; k < 3; ++k) {
                printf("\t[%f, %f, %f]", connFace->p_vertex(k)->vertex().x(), connFace->p_vertex(k)->vertex().y(), connFace->p_vertex(k)->vertex().z());
            }
            printf("\n");
        }
        printf("\n");
        
    }
    
//    ClipperLib::Clipper clip;
//    ClipperLib::Path poly;
//    poly << ClipperLib::IntPoint(0, 0) << ClipperLib::IntPoint(0, 10) << ClipperLib::IntPoint(10, 10) << ClipperLib::IntPoint(10, 0);
//    printf("area: %f\n", ClipperLib::Area(poly));
//    printf("in poly: %s\n", ClipperLib::PointInPolygon(ClipperLib::IntPoint(5, 5), poly) ? "true" : "false");
    
    ClipperLib::Clipper clipper;
    ClipperLib::Paths polys;
    ClipperLib::Path p1;
    ClipperLib::Path p2;
    ClipperLib::Path p3;
    ClipperLib::Path p4;
    
    p1 << ClipperLib::IntPoint(10, 10) << ClipperLib::IntPoint(100, 10) << ClipperLib::IntPoint(100, 100) << ClipperLib::IntPoint(10, 100);
    p2 << ClipperLib::IntPoint(20, 20) << ClipperLib::IntPoint(20, 90) << ClipperLib::IntPoint(90, 90) << ClipperLib::IntPoint(90, 20);
    p3 << ClipperLib::IntPoint(30, 30) << ClipperLib::IntPoint(50, 30) << ClipperLib::IntPoint(50, 50) << ClipperLib::IntPoint(30, 50);
    p4 << ClipperLib::IntPoint(60, 60) << ClipperLib::IntPoint(80, 60) << ClipperLib::IntPoint(80, 80) << ClipperLib::IntPoint(60, 80);
    
    clipper.AddPath(p4, ClipperLib::ptSubject, true);
    clipper.AddPath(p1, ClipperLib::ptSubject, true);
    clipper.AddPath(p3, ClipperLib::ptSubject, true);
    clipper.AddPath(p2, ClipperLib::ptSubject, true);
    
    ClipperLib::PolyTree solution;
    
    clipper.Execute(ClipperLib::ctUnion, solution);
    solution.Total();
    
    Slice slice = Slicer(p_mesh).slice(Plane(Vector3D(0, 0, 1), 100));
    slice.p_islands();
    
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
