//
//  main.cpp
//  generator
//
//  Created by Ethan Coeytaux on 10/6/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

// #define RUN_TESTS

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
#include <queue>
#include "BuildMapToMATLAB.hpp"
#include "Slicer.hpp"
#include "DirectedGraph.hpp"
//end debugging

using namespace mapmqp;
using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
#ifdef RUN_TESTS
    //run tests
    mapmqp::writeLog(mapmqp::INFO, "running tests...");
    int catch_argc = argc;
    char ** catch_argvs;
    char * catch_a = new char[argc];
    catch_argvs = &catch_a;
    //catch_main(catch_argc, catch_argvs);
    //return 0;
#endif
    
    DirectedGraph<int> graph;
    
    int n0 = graph.addVertex(0);
    int n1 = graph.addVertex(1);
    int n2 = graph.addVertex(2);
    int n3 = graph.addVertex(3);
    int n4 = graph.addVertex(4);
    int n5 = graph.addVertex(5);
    int n6 = graph.addVertex(6);
    int n7 = graph.addVertex(7);
    int n8 = graph.addVertex(8);
    
    graph.addDirectedEdge(0, 1);
    graph.addDirectedEdge(1, 2);
    graph.addDirectedEdge(2, 3);
    graph.addDirectedEdge(3, 4);
    graph.addDirectedEdge(4, 5);
    graph.addDirectedEdge(5, 1);
    graph.addDirectedEdge(2, 6);
    graph.addDirectedEdge(6, 7);
    graph.addDirectedEdge(7, 3);
    graph.addDirectedEdge(7, 8);
    graph.addDirectedEdge(8, 2);
    
    vector<vector<int>> q = graph.findCycles();
    
    for (auto & x : q) {
        printf("cycle: ");
        for (auto & y : x) {
            printf("%i", y);
        }
        printf("\n");
    }
    
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
    if (argc < 2) {
        writeLog(ERROR, "path to the STL file must be supplied");
        return 0;
    }
    
    std::shared_ptr<mapmqp::Mesh> p_mesh = ProcessSTL::constructMeshFromSTL(argv[1]);
    const std::vector<std::shared_ptr<mapmqp::Mesh::Face>> meshFaces = p_mesh->p_faces();
    // for (uint32_t i = 0; i < meshFaces.size(); ++i) {
    //     const std::shared_ptr<mapmqp::Mesh::Face> currFace = meshFaces[i];
        
    //     printf("Face vertices are:\n");
    //     for (uint16_t j = 0; j < 3; ++j) {
    //         printf("\t[%f, %f, %f]", currFace->p_vertex(j)->vertex().x(), currFace->p_vertex(j)->vertex().y(), currFace->p_vertex(j)->vertex().z());
    //     }
    //     printf("\n");
    //     printf("Connected faces are:\n");
    //     for (uint16_t j = 0; j < 3; ++j) {
    //         printf("%d.)", j);
    //         const std::shared_ptr<const mapmqp::Mesh::Face> connFace = currFace->p_connectedFace(j);
    //         for (uint16_t k = 0; k < 3; ++k) {
    //             printf("\t[%f, %f, %f]", connFace->p_vertex(k)->vertex().x(), connFace->p_vertex(k)->vertex().y(), connFace->p_vertex(k)->vertex().z());
    //         }
    //         printf("\n");
    //     }
    //     printf("\n");
        
    // }
    
    ProcessSTL::constructSTLfromMesh(*p_mesh, "debug/TestSTL.STL");
    
    mapmqp::BuildMap map(p_mesh);
    map.solve();
    BuildMapToMATLAB::parse("debug/buildmap-plane.m", map, BuildMapToMATLAB::PLANE, 25);
    BuildMapToMATLAB::parse("debug/buildmap-sphere.m", map, BuildMapToMATLAB::SPHERE, 25);
    BuildMapToMATLAB::parse("debug/buildmap-sphere-smooth.m", map, BuildMapToMATLAB::SPHERE_SMOOTH, 25);
    
    map.checkVector(mapmqp::Vector3D(mapmqp::Angle(0), mapmqp::Angle(0)));
    
    double originalArea = A_AXIS_DISCRETE_POINTS * B_AXIS_DISCRETE_POINTS;
    printf("original area: %f\n", originalArea);
    
    double area = map.area();
    printf("area: %f\n", area);
    printf("area difference: %f\n", originalArea - area);
    
    mapmqp::Vector3D validVector = map.findValidVector();
    printf("valid vector(theta:%f, phi:%f)\n", validVector.theta().val(), validVector.phi().val());
    
    //TODO check to make sure this value is correct
    mapmqp::Vector3D bestVector = map.findBestVector();
    printf("best vector(theta:%f, phi:%f)\n", bestVector.theta().val(), bestVector.phi().val());
    
    return 0;
}
