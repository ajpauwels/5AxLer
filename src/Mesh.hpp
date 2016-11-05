//
//  Mesh.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 10/30/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <string>

#include "Vector3D.hpp"
#include "Polygon.hpp"
#include "Island.hpp"

namespace mapmqp {
    //forward declarations to use pointers
    class MeshVertex;
    class MeshFace;
    
    // Mesh class declaration

    class Mesh {
    public:
        Mesh();
        
        const std::vector<std::shared_ptr<MeshVertex>> & p_vertices() const;
        const std::vector<std::shared_ptr<MeshFace>> & p_faces() const;

        void addVertex(std::shared_ptr<MeshVertex> p_vertex);
        void addFace(std::shared_ptr<MeshFace> p_face);
        
        std::pair<std::vector<Island>, std::vector<std::shared_ptr<const MeshFace>>> planeIntersection(const Plane & plane, std::vector<std::shared_ptr<const MeshFace>> p_faces) const;
        
    private:
        
        std::vector<std::shared_ptr<MeshVertex>> p_vertices_;
        std::vector<std::shared_ptr<MeshFace>> p_faces_;
        
        std::vector<std::shared_ptr<MeshVertex>> p_lowestVertices_;
    };

    // MeshVertex class declaration
    
    class MeshVertex {
        friend class Mesh;
    public:
        MeshVertex(const Vector3D & vertex);
        
        // Getters
        Vector3D vertex() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_faces() const;

        // Adds a face to the vector of connected faces
        void addConnectedFace(std::shared_ptr<MeshFace> p_face);
        
    private:
        Vector3D vertex_;
        std::vector<std::shared_ptr<const MeshFace>> p_faces_; //all faces that have this vertex as a vertex
    };

    // MeshEdge class declaration
    
    class MeshEdge {
        friend class Mesh;
    public:
        MeshEdge(std::shared_ptr<const MeshVertex> p_vertex1, std::shared_ptr<const MeshVertex> p_vertex2);

        void addFace(std::shared_ptr<MeshFace> p_face);
        void other(std::shared_ptr<MeshVertex> p_vertex);
        void other(std::shared_ptr<MeshFace> p_face);

        // Getters
        const std::shared_ptr<const MeshVertex> p_vertex(uint16_t v) const;

        // Operator overloads
        bool operator==(const MeshEdge & edge) const;
        
    private:
        bool faceAdded = false;
        std::shared_ptr<const MeshVertex> p_vertices_[2];
    };
    
    class MeshFace {
        friend class Mesh;
    public:
        MeshFace(std::shared_ptr<const MeshVertex> p_vertex1, std::shared_ptr<const MeshVertex> p_vertex2, std::shared_ptr<const MeshVertex> p_vertex3); //TODO add normal checking

        // Getters
        const std::shared_ptr<const MeshVertex> p_vertex(uint16_t v) const;
        const std::shared_ptr<const MeshFace> p_connectedFace(uint16_t f) const;
        Vector3D p_normal();
        
        //returns whether or not the face intersections with Plane
        bool intersectsPlane(const Plane & plane) const;
        
        //returns whether or not the entire face lays is on Plane (that is, all vertices of face lie on the plane)
        bool liesOnPlane(const Plane & plane) const;

        //if the face intersections with Plane and does not lie entirely on the plane, returns the line of intersection between the face and the plane, pair.first->pair.second is going counterclockwise
        std::pair<Vector3D, Vector3D> planeIntersection(const Plane & plane) const;

        // Add a connecting face to this face
        void connect(std::shared_ptr<MeshFace> p_face, uint16_t edgeIndex);

        // Get the edge index of the two given vertices
        int16_t getEdgeIndex(std::shared_ptr<MeshEdge> p_edge);

        // Operator overloads
        bool operator==(const MeshFace & face) const;
        
    private:
        //x, y, z vertices in counter-clockwise order
        std::shared_ptr<const MeshVertex> p_vertices_[3];

        //faces that share the x/y, y/z, and z/x edges
        std::shared_ptr<const MeshFace> p_faces_[3];
        
        double area_;
        Vector3D normal_;
    };
}

#endif /* Mesh_hpp */
