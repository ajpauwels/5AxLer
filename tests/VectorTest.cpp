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

TEST_CASE("construct multiple Vector3Ds and take the cross and dot products", "[Vector3D]") {
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
    
    Vector3D v5(Angle(0), Angle(M_PI / 2), 5);
    Vector3D v6(Angle(5 * M_PI / 4), Angle(-2 * M_PI / 3));
    SECTION("test Vector3Ds Angles") {
        REQUIRE(doubleEquals(v1.theta().val(), 0.674740942, 0.0000001));
        REQUIRE(doubleEquals(v2.theta().val(), 1.57079632679, 0.0000001));
        REQUIRE(doubleEquals(v3.phi().val(), 0.78539816339, 0.0000001));
        REQUIRE(doubleEquals(v5.magnitude(), 5, 0.0000001));
        REQUIRE(doubleEquals(v5.x(), 5, 0.0000001));
        REQUIRE(doubleEquals(v5.y(), 0, 0.0000001));
        REQUIRE(doubleEquals(v5.z(), 0, 0.0000001));
        REQUIRE(doubleEquals(v6.theta().val(), M_PI / 4, 0.0000001));
        REQUIRE(doubleEquals(v6.phi().val(), 2 * M_PI / 3, 0.0000001));
    }
}
