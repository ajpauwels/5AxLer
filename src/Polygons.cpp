//
//  Polygons.cpp
//  5AxLer
//
//  Created by Alexandre Pauwels on 11/28/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Polygon.hpp"

#include <cmath>

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

Polygons::Polygons(const std::vector<Polygon> & polygons) : m_polygons(polygons);

std::vector<Polygon> Polygons::m_polygons() {
	return m_polygons;
}