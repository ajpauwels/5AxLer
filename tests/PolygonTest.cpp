//
//  PolygonTest.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/1/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "../libs/Catch/catch.hpp"

#include "../src/Polygon.hpp"

using namespace mapmqp;

TEST_CASE("test Polygon construction and intersection", "[polygon]") {
    SECTION("test square on x/y plane") {
        std::vector<Vector3D> squarePoints;
        squarePoints.push_back(Vector3D(4, 4));
        squarePoints.push_back(Vector3D(-1, 4));
        squarePoints.push_back(Vector3D(-1, -1));
        squarePoints.push_back(Vector3D(4, -1));
        Polygon square(squarePoints);
        REQUIRE(square.area() == 25);
    }
    
    SECTION("test square on x/z plane") {
        std::vector<Vector3D> squarePoints;
        squarePoints.push_back(Vector3D(4, 0, 4));
        squarePoints.push_back(Vector3D(-1, 0, 4));
        squarePoints.push_back(Vector3D(-1, 0, -1));
        squarePoints.push_back(Vector3D(4, 0, -1));
        Polygon square(squarePoints);
        REQUIRE(square.area() == 25);
    }
    
    /*SECTION("test square on non-axis aligned plane") {
        std::vector<Vector3D> squarePoints;
        squarePoints.push_back(Vector3D(10, 0, 5));
        squarePoints.push_back(Vector3D(0, 10, 5));
        squarePoints.push_back(Vector3D(0, 10, -5));
        squarePoints.push_back(Vector3D(10, 0, -5));
        //parallelogramPoints.push_back(Vector3D(10, 10, -5));
        Polygon square(squarePoints);
        
        REQUIRE(square.area() == 100);
        
        SECTION("test square mapped to x/y plane") {
            Polygon mappedSquare = square.mapToXYPlane();
            
            REQUIRE(mappedSquare.area() == 100);
        }
    }
    
    SECTION("test parallelogram on non-axis aligned plane") {
        std::vector<Vector3D> parallelogramPoints;
        parallelogramPoints.push_back(Vector3D(5, 0, 0));
        parallelogramPoints.push_back(Vector3D(0, 0, 5));
        parallelogramPoints.push_back(Vector3D(0, 5, 0));
        //parallelogramPoints.push_back(Vector3D(10, 10, -5));
        Polygon parallelogram(parallelogramPoints);
        
        REQUIRE(parallelogram.area() == 64);
        
        SECTION("test parallelogram mapped to x/y plane") {
            Polygon mappedParallelogram = parallelogram.mapToXYPlane();
            
            REQUIRE(mappedParallelogram.area() == 64);
        }
    }*/
}
