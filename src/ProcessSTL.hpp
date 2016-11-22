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
        static std::shared_ptr<Mesh> constructMeshFromSTL(std::string stlFilePath);
        static bool constructSTLfromMesh(const Mesh & mesh, std::string stlFilePath);

	private:
		/**
		 * Creates a hash value for a Mesh::Vertex using the vertex's vector
		 */
		struct Vector3DHash {
		    std::size_t operator()(const Vector3D & v) const {
                long x = (long)v.x();
		        long y = (long)v.y();
		        long z = (long)v.z();
		        
		        int hashVal = (uint32_t)(x ^ (x >> 32));
		        hashVal = 31 * hashVal + (uint32_t)(y ^ (y >> 32));
		        hashVal = 31 * hashVal + (uint32_t)(z ^ (z >> 32));
		        return std::hash<uint32_t>()(hashVal);
		    }
		};

		/**
		 * Creates a hash value for a Mesh::Edge object using its vertices
		 */
		struct MeshEdgePtrHash {
		    std::size_t operator()(const std::shared_ptr<Mesh::Edge> & p_me) const {
		        Vector3DHash v3hasher;

		        int hash = 17;
		        hash = hash * 31 + v3hasher(p_me->p_vertex(0)->vertex());
		        hash = hash * 31 + v3hasher(p_me->p_vertex(0)->vertex()); //TODO should this be accessing the second vertex instead?
		        return std::hash<int>()(hash);
		    }
		};
        
		struct MeshEdgePtrEquality {
		    bool operator()(const std::shared_ptr<Mesh::Edge> & p_edge1, const std::shared_ptr<Mesh::Edge> & p_edge2) const {
		        return *p_edge1 == *p_edge2;
		    }
		};

        static std::shared_ptr<Mesh::Vertex> addMeshVertex(std::shared_ptr<Mesh> p_mesh, std::shared_ptr<Mesh::Vertex> p_vertex, std::unordered_map<Vector3D, std::shared_ptr<Mesh::Vertex>, ProcessSTL::Vector3DHash> & mapped_p_vertices);
        static void addMeshFace(std::shared_ptr<Mesh> p_mesh, std::shared_ptr<Mesh::Face> p_face, std::unordered_map<std::shared_ptr<Mesh::Edge>, std::shared_ptr<Mesh::Face>, ProcessSTL::MeshEdgePtrHash, ProcessSTL::MeshEdgePtrEquality> & mapped_p_edges);
        
        static bool getFileHandlerIn(std::ifstream& file, std::string filePath);
        static bool getFileHandlerOut(std::ofstream& file, std::string filePath);
	};
}

#endif
