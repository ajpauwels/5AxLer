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
#include <string.h>

#include "Vector3D.hpp"

namespace mapmqp {
    //forward declarations to use pointers
    class MeshVertex;
    class MeshFace;
    
    // Mesh class declaration

    class Mesh {
    public:
        Mesh();
        
        const std::vector<std::shared_ptr<MeshVertex>> & vertices();
        const std::vector<std::shared_ptr<MeshFace>> & faces();

        void addVertex(std::shared_ptr<MeshVertex> vertex);
        void addFace(std::shared_ptr<MeshFace> face);
        
    private:
        std::vector<std::shared_ptr<MeshVertex>> vertices_;
        std::vector<std::shared_ptr<MeshFace>> faces_;
        
        std::vector<std::shared_ptr<MeshVertex>> lowestVertices_;
    };

    // MeshVertex class declaration
    
    class MeshVertex {
        friend class Mesh;
    public:
        MeshVertex(Vector3D vertex);
        
        // Getters
        Vector3D vertex() const;
        const std::vector<std::shared_ptr<MeshFace>> & p_faces() const;

        // Adds a face to the vector of connected faces
        void addConnectedFace(std::shared_ptr<MeshFace> face);
        
    private:
        Vector3D vertex_;
        std::vector<std::shared_ptr<MeshFace>> p_faces_; //all faces that have this vertex as a vertex
    };

    // MeshEdge class declaration
    
    class MeshEdge {
        friend class Mesh;
    public:
        MeshEdge(std::shared_ptr<MeshVertex> v1, std::shared_ptr<MeshVertex> v2);

        void addFace(std::shared_ptr<MeshFace> p_face);
        void other(std::shared_ptr<MeshVertex> p_vertex);
        void other(std::shared_ptr<MeshFace> p_face);

        // Getters
        const std::shared_ptr<MeshVertex> getVertex(uint16_t v) const;

        // Operator overloads
        bool operator==(const MeshEdge & edge) const;
        
    private:
        bool faceAdded = false;
        std::shared_ptr<MeshVertex> p_vertices_[2];
    };
    
    class MeshFace {
        friend class Mesh;
    public:
        MeshFace(std::shared_ptr<MeshVertex> v1, std::shared_ptr<MeshVertex> v2, std::shared_ptr<MeshVertex> v3); //TODO add normal checking
        
        // Getters
        const std::shared_ptr<MeshVertex> getVertex(uint16_t v) const;
        const std::shared_ptr<MeshFace> getConnectedFace(uint16_t f) const;
        
        //returns whether or not the face intersections with a plane with a normal of planeNormal and contains the coordinate pointOnPoint
        bool intersectsPlane(Vector3D planeNormal, Vector3D pointOnPlane);
        
        //returns whether or not the entire face lays is on a plane with a normal of planeNormal and contains the coordinate pointOnPoint (that is, all vertices of face lie on the plane)
        bool liesOnPlane(Vector3D planeNormal, Vector3D pointOnPlane);

        //if the face intersections with a plane with a normal of planeNormal and contains the coordinate pointOnPoint and does not lie entirely on the plane, returns the line of intersection between the face and the plane
        std::pair<Vector3D, Vector3D> planeIntersection(Vector3D planeNormal, Vector3D pointOnPlane);

        // Add a connecting face to this face
        void connect(std::shared_ptr<MeshFace> face, uint16_t edgeIndex);

        // Get the edge index of the two given vertices
        int16_t getEdgeIndex(std::shared_ptr<MeshEdge> edge);

        // Operator overloads
        bool operator==(const MeshFace & face) const;
        
    private:
        // Takes in an index value and returns the next, counterclockwise, 0-1-2-0...
        uint16_t nextCircularIndex(uint16_t curr);

        //x, y, z vertices in counter-clockwise order
        std::shared_ptr<MeshVertex> p_vertices_[3];

        //faces that share the x/y, y/z, and z/x edges
        std::shared_ptr<MeshFace> p_faces_[3];
        
        double area_;
        Vector3D normal_;
    };
}

#endif /* Mesh_hpp */
