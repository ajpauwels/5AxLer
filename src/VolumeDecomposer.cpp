//
//  VolumeDecomposer.cpp
//  5AxLer
//
//  Created by Alexandre Pauwels on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "VolumeDecomposer.hpp"

using namespace mapmqp;
using namespace std;

VolumeDecomposer::VolumeDecomposer() {}

vector<shared_ptr<Mesh>> VolumeDecomposer::run(shared_ptr<Mesh> p_mesh, Plane orientation) {
	Slicer slicer = new Slicer(p_mesh);

	for (int i = 0; i < 1000; ++i) {
		Plane firstPlane = new Plane(orientation, i * SLICE_THICKNESS);
		Plane secondPlane = new Plane(orientation, (i + 1) * SLICE_THICKNESS);
		Slice comparisonSlice = slicer.slice(firstPlane);
		Slice currSlice = slicer.slice(secondPlane);
	}
}
