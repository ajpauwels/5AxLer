/**
 * Takes in a Mesh object and decomposes it into a vector of Mesh
 * objects that are printable in one direction
 *
 * Created by Alexandre Pauwels on 11/4/2016
 * Copyright © 2016 MAP MQP. All rights reserved.
 */

#ifndef VolumeDecomposer_hpp
#define VolumeDecomposer_hpp

#include "Mesh.hpp"

namespace mapmqp {
	// Class definition
	class VolumeDecomposer {
	public:
		// Constructor
		VolumeDecomposer(std::shared_ptr<Mesh> p_mesh);

		// Returns the decomposed array of sub-meshes
		// std::vector<std::shared_ptr<Mesh>> run();

	private:
		std::shared_ptr<Mesh> m_p_mesh;
	};
}

#endif /* VolumeDecomposer_hpp */