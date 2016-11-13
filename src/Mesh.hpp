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
#include "Plane.hpp"
#include "Polygon.hpp"

namespace mapmqp {
    class Mesh {
    public:
        //sub-class forward declarations
        class Vertex;
        class Edge;
        class Face;

        Mesh();
        
        const std::vector<std::shared_ptr<Vertex>> & p_vertices() const;
        const std::vector<std::shared_ptr<Face>> & p_faces() const;

        void addVertex(std::shared_ptr<Vertex> p_vertex);
        void addFace(std::shared_ptr<Face> p_face);
        
        void transform(void (*transformFnc)(Vector3D & v));
        
    private:
        std::vector<std::shared_ptr<Vertex>> m_p_vertices;
        std::vector<std::shared_ptr<Face>> m_p_faces;
        
        std::vector<std::shared_ptr<Vertex>> m_p_lowestVertices;
        
    public:
        //sub-class declarations
        
        // Mesh::Vertex class declaration
        
        class Vertex {
            friend class Mesh;
        public:
            Vertex(const Vector3D & vertex);
            
            // Getters
            Vector3D vertex() const;
            const std::vector<std::shared_ptr<const Face>> & p_faces() const;
            
            // Adds a face to the vector of connected faces
            void addConnectedFace(std::shared_ptr<Face> p_face);
            
            std::string toString() const;
            
        private:
            Vector3D m_vertex;
            std::vector<std::shared_ptr<const Face>> m_p_faces; //all faces that have this vertex as a vertex
        };
        
        // Mesh::Edge class declaration
        
        class Edge {
            friend class Mesh;
        public:
            Edge(std::shared_ptr<const Vertex> p_vertex1, std::shared_ptr<const Vertex> p_vertex2);
            
            void addFace(std::shared_ptr<Face> p_face);
            void other(std::shared_ptr<Vertex> p_vertex);
            void other(std::shared_ptr<Face> p_face);
            
            // Getters
            const std::shared_ptr<const Vertex> p_vertex(uint16_t v) const;
            
            std::string toString() const;
            
            // Operator overloads
            bool operator==(const Edge & edge) const;
            
        private:
            bool m_faceAdded = false;
            std::shared_ptr<const Vertex> m_p_vertices[2];
        };
        
        //Mesh::Face declaration
        
        class Face {
            friend class Mesh;
        public:
            Face(std::shared_ptr<const Vertex> p_vertex1, std::shared_ptr<const Vertex> p_vertex2, std::shared_ptr<const Vertex> p_vertex3); //TODO add normal checking
            
            // Getters
            const std::shared_ptr<const Vertex> p_vertex(uint16_t v) const;
            const std::shared_ptr<const Face> p_connectedFace(uint16_t f) const;
            double area() const;
            const Vector3D & normal() const;
            
            //returns whether or not the face intersections with Plane
            bool intersectsPlane(const Plane & plane) const;
            
            //returns whether or not the entire face lays is on Plane (that is, all vertices of face lie on the plane)
            bool liesOnPlane(const Plane & plane) const;
            
            //if the face intersections with Plane and does not lie entirely on the plane, returns the line of intersection between the face and the plane, pair.first->pair.second is going counterclockwise
            std::pair<Vector3D, Vector3D> planeIntersection(const Plane & plane) const;
            
            // Add a connecting face to this face
            void connect(std::shared_ptr<Face> p_face, uint16_t edgeIndex);
            
            // Get the edge index of the two given vertices
            int16_t getEdgeIndex(std::shared_ptr<Edge> p_edge);
            
            std::string toString() const;
            
            // Operator overloads
            bool operator==(const Face & face) const;
            
        private:
            //x, y, z vertices in counter-clockwise order
            std::shared_ptr<const Vertex> m_p_vertices[3];
            
            //faces that share the x/y, y/z, and z/x edges
            std::shared_ptr<const Face> m_p_faces[3];
            
            double m_area;
            Vector3D m_normal;
        };
    };
}

#endif /* Mesh_hpp */
