//
//  AngleTest.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/2/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "../libs/Catch/catch.hpp"

#include "../src/Utility.hpp"
#include "../src/Angle.hpp"

using namespace mapmqp;

TEST_CASE("construct multiple Angles", "[angle]") {
    Angle a1(M_PI);
    Angle a2(M_PI * 2);
    Angle a3(M_PI * 3 / 2);
    Angle a4(M_PI / 3);
    Angle a5(-M_PI * 4 / 5);
    
    REQUIRE(doubleEquals(a1.val(), M_PI, 0.0000001));
    REQUIRE(doubleEquals(a2.val(), 0, 0.0000001)); //checks to make sure value stays within [0, 2pi)
    REQUIRE(doubleEquals(a1.sinVal(), 0, 0.0000001));
    REQUIRE(doubleEquals(a1.cosVal(), -1, 0.0000001));
    REQUIRE(doubleEquals(a2.sinVal(), 0, 0.0000001));
    REQUIRE(doubleEquals(a2.cosVal(), 1, 0.0000001));
    REQUIRE(doubleEquals(a3.sinVal(), -1, 0.0000001));
    REQUIRE(doubleEquals(a3.cosVal(), 0, 0.0000001));
    REQUIRE(doubleEquals(a4.sinVal(), 0.866025403784438, 0.0000001));
    REQUIRE(doubleEquals(a4.cosVal(), 0.5, 0.0000001));
    REQUIRE(doubleEquals(a5.val(), M_PI * 6 / 5, 0.0000001)); //checks to make sure value wraps in negative direction
}
