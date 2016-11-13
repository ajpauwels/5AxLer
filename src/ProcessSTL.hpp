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
		 * Creates a hash value for a MeshVertex using the vertex's vector
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
		 * Creates a hash value for a MeshEdge object using its vertices
		 */
		struct MeshEdgePtrHash {
		    std::size_t operator()(const std::shared_ptr<MeshEdge> & p_me) const {
		        Vector3DHash v3hasher;

		        int hash = 17;
		        hash = hash * 31 + v3hasher(p_me->p_vertex(0)->vertex());
		        hash = hash * 31 + v3hasher(p_me->p_vertex(0)->vertex());
		        return std::hash<int>()(hash);
		    }
		};

		struct MeshEdgePtrEquality {
		    bool operator()(const std::shared_ptr<MeshEdge> & p_edge1, const std::shared_ptr<MeshEdge> & p_edge2) const {
		        return *p_edge1 == *p_edge2;
		    }
		};

		static std::shared_ptr<Mesh> s_p_mesh;
		static std::unordered_map<Vector3D, std::shared_ptr<MeshVertex>, Vector3DHash> s_mapped_p_vertices;
        static std::unordered_map<std::shared_ptr<MeshEdge>, std::shared_ptr<MeshFace>, MeshEdgePtrHash, MeshEdgePtrEquality> s_mapped_p_edges;
        static std::vector<std::shared_ptr<MeshVertex>> s_p_lowestVertices;

        static void resetVariables();
		static std::shared_ptr<MeshVertex> addMeshVertex(std::shared_ptr<MeshVertex> p_vertex);
        static void addMeshFace(std::shared_ptr<MeshFace> p_face);
        
        static bool getFileHandlerIn(std::ifstream& file, std::string filePath);
        static bool getFileHandlerOut(std::ofstream& file, std::string filePath);
	};
}

#endif
