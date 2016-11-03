/**
 * Takes in an STL file and process it into a complete Mesh object
 *
 * Created by Alex Pauwels on 11/1/2016
 * Copyright © 2016 MAP MQP. All rights reserved.
 */

#ifndef ProcessSTL_hpp
#define ProcessSTL_hpp

#include <string>
#include <unordered_map>
#include "Mesh.hpp"

namespace mapmqp {
	class ProcessSTL {
	public:
		ProcessSTL(std::string stlFiles);
		bool run();

	private:
		/**
		 * Creates a hash value for a MeshVertex using the vertex's vector
		 */
		struct Vector3DHash {
		    std::size_t operator()(const Vector3D & v) const { //TODO use sizeof(long) instead? should they be longs or ints
		        long x = (long)v.x();
		        long y = (long)v.y();
		        long z = (long)v.z();
		        
		        int hashVal = (int)(x ^ (x >> 32));
		        hashVal = 31 * hashVal + (int)(y ^ (y >> 32));
		        hashVal = 31 * hashVal + (int)(z ^ (z >> 32));
		        return std::hash<int>()(hashVal);
		    }
		};

		/**
		 * Creates a hash value for a MeshEdge object using its vertices
		 */
		struct MeshEdgeHash {
		    std::size_t operator()(const std::shared_ptr<MeshEdge> & me) const {
		        Vector3DHash v3hasher;

		        int hash = 17;
		        hash = hash * 31 + v3hasher(me->getVertex(0)->vertex());
		        hash = hash * 31 + v3hasher(me->getVertex(0)->vertex());
		        return std::hash<int>()(hash);
		    }
		};

		std::shared_ptr<Mesh> p_mesh_;
		std::string stlFile_;
		std::unordered_map<Vector3D, std::shared_ptr<MeshVertex>, Vector3DHash> mappedVertices_;
        std::unordered_map<std::shared_ptr<MeshEdge>, std::shared_ptr<MeshFace>, MeshEdgeHash> mappedEdges_;
        std::vector<std::shared_ptr<MeshVertex>> lowestVertices_;

		bool getFileHandler(std::ifstream& file);
		std::shared_ptr<MeshVertex> addMeshVertex(std::shared_ptr<MeshVertex> vertex);
		void addMeshFace(std::shared_ptr<MeshFace> face);
		void constructMeshFromSTL();
	};
}

#endif