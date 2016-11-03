//
//  PolygonTest.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/1/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "../libs/Catch/catch.hpp"

#include "../src/Utility.hpp"
#include "../src/Polygon.hpp"

using namespace mapmqp;

TEST_CASE("test Polygon construction and intersection", "[Polygon]") {
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
    
    SECTION("test square on non-axis aligned plane") {
        Vector3D origin(1, 1, 1);
        Vector3D xAxis(Angle(5 * M_PI / 4), Angle(fabs(origin.phi().val() - (M_PI / 2))));
        Vector3D yAxis = Vector3D::crossProduct(origin, xAxis);
        
        REQUIRE(doubleEquals(Vector3D::dotProduct(origin, xAxis), 0, 0.0000001));
        REQUIRE(doubleEquals(Vector3D::dotProduct(origin, yAxis), 0, 0.0000001));
        REQUIRE(doubleEquals(Vector3D::dotProduct(xAxis, yAxis), 0, 0.0000001));
        
        xAxis.normalize(10);
        yAxis.normalize(10);
        
        std::vector<Vector3D> squarePoints;
        squarePoints.push_back(origin);
        squarePoints.push_back(origin + xAxis);
        squarePoints.push_back(origin + xAxis - yAxis);
        squarePoints.push_back(origin - yAxis);
        Polygon square(squarePoints);
        
        REQUIRE(square.area() == 100);
        
        SECTION("test square mapped to x/y plane") {
            Polygon mappedSquare = square.mapToXYPlane();
            
            REQUIRE(mappedSquare.area() == 100);
            REQUIRE(mappedSquare.points()[0].equals(Vector3D(0, 0, 0), 0.0000001));
            REQUIRE(mappedSquare.points()[1].equals(Vector3D(10, 0, 0), 0.0000001));
            REQUIRE(mappedSquare.points()[2].equals(Vector3D(10, -10, 0), 0.0000001));
            REQUIRE(mappedSquare.points()[3].equals(Vector3D(0, -10, 0), 0.0000001));
        }
    }
    
    SECTION("test parallelogram on non-axis aligned plane") {
        std::vector<Vector3D> parallelogramPoints;
        parallelogramPoints.push_back(Vector3D(5, 0, 0));
        parallelogramPoints.push_back(Vector3D(0, 0, 5));
        parallelogramPoints.push_back(Vector3D(0, 5, 0));
        Polygon parallelogram(parallelogramPoints);
        
        REQUIRE(doubleEquals(parallelogram.area(), 21.6506350946, 0.0001));
        
        SECTION("test parallelogram mapped to x/y plane") {
            Polygon mappedParallelogram = parallelogram.mapToXYPlane();
            
            REQUIRE(doubleEquals(parallelogram.area(), mappedParallelogram.area(), 0.0000001));
        }
    }
}
