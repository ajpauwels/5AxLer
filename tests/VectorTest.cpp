//
//  VectorTest.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/2/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "../libs/Catch/catch.hpp"

#include "../src/Utility.hpp"
#include "../src/Vector3D.hpp"

using namespace mapmqp;

TEST_CASE("construct multiple Vector3Ds and take the cross and dot products", "[vector]") {
    Vector3D v1(5, 4, 3);
    Vector3D v2(0, 6, 70);
    Vector3D v3(5, 0, 5);
    Vector3D v4(0, 5, 0);
    
    SECTION("test magnitudes") {
        REQUIRE(doubleEquals(v1.magnitude(), 7.07106781186548, 0.0000001));
        REQUIRE(doubleEquals(v2.magnitude(), 70.2566722810012, 0.0000001));
        
        REQUIRE(v1 + v2 == v2 + v1);
        REQUIRE(v1 - v2 != v2 - v1);
        
        REQUIRE(v1 + v2 == Vector3D(5, 10, 73));
        REQUIRE(v2 - v1 == Vector3D(-5, 2, 67));
        
        SECTION("test normalization") {
            v1.normalize();
            v2.normalize();
            REQUIRE(doubleEquals(v1.magnitude(), 1, 0.0000001));
            REQUIRE(doubleEquals(v2.magnitude(), 1, 0.0000001));
        }
    }
    
    SECTION("test dot product") {
        REQUIRE(Vector3D::dotProduct(v1, v2) == Vector3D::dotProduct(v2, v1));
        REQUIRE(Vector3D::dotProduct(v1, v2) == 234);
        
        SECTION("test angle between Vector3Ds using dot product") {
            v1.normalize();
            v2.normalize();
            
            REQUIRE(doubleEquals(acos(Vector3D::dotProduct(v1, v2)), 1.0803447694, 0.0000001));
        }
    }
    
    SECTION("test cross product") {
        REQUIRE(Vector3D::crossProduct(v3, v3) == Vector3D(0, 0, 0));
        REQUIRE(Vector3D::crossProduct(v3, v4) == Vector3D(-25, 0, 25));
    }
}
