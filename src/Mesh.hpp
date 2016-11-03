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

#include "Vector3D.hpp"

namespace mapmqp {
    //forward declarations to use pointers
    class MeshVertex;
    class MeshFace;
    
    class Mesh {
    public:
        Mesh();
		
		void constructSTLFromMesh(std::string stlFilePath);
        void constructMeshFromSTL(std::string stlFilePath);
        
        const std::vector<std::shared_ptr<MeshVertex>> & p_vertices();
        const std::vector<std::shared_ptr<MeshFace>> & p_faces();
        
    private:
        std::vector<std::shared_ptr<MeshVertex>> p_vertices_;
        std::vector<std::shared_ptr<MeshFace>> p_faces_;
        
        std::vector<std::shared_ptr<MeshVertex>> p_lowestVertices_;
    };
    
    class MeshVertex {
        friend class Mesh;
    public:
        MeshVertex(const Vector3D & vertex);
        
        //getters
        Vector3D vertex() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_faces() const;
        
    private:
        Vector3D vertex_;
        std::vector<std::shared_ptr<const MeshFace>> p_faces_; //all faces that have this vertex as a vertex
    };
    
    class MeshEdge {
        friend class Mesh;
    public:
        MeshEdge(std::shared_ptr<const MeshVertex> p_vertex1, std::shared_ptr<const MeshVertex> p_vertex2);
        
    private:
        std::shared_ptr<const MeshVertex> p_v1_ = nullptr, p_v2_ = nullptr;
    };
    
    class MeshFace {
        friend class Mesh;
    public:
        MeshFace(std::shared_ptr<const MeshVertex> p_vertex1, std::shared_ptr<const MeshVertex> p_vertex2, std::shared_ptr<const MeshVertex> p_vertex3); //TODO add normal checking
        
        //getters
        std::shared_ptr<const MeshVertex> p_vertex1();
        std::shared_ptr<const MeshVertex> p_vertex2();
        std::shared_ptr<const MeshVertex> p_vertex3();
        std::shared_ptr<const MeshFace> p_face12();
        std::shared_ptr<const MeshFace> p_face23();
        std::shared_ptr<const MeshFace> p_face31();
		Vector3D p_normal();
        
        //returns whether or not the face intersections with a plane with a normal of planeNormal and contains the coordinate pointOnPoint
        bool intersectsPlane(const Vector3D & planeNormal, const Vector3D & pointOnPlane);
        
        //returns whether or not the entire face lays is on a plane with a normal of planeNormal and contains the coordinate pointOnPoint (that is, all vertices of face lie on the plane)
        bool liesOnPlane(const Vector3D & planeNormal, const Vector3D & pointOnPlane);
        //if the face intersections with a plane with a normal of planeNormal and contains the coordinate pointOnPoint and does not lie entirely on the plane, returns the line of intersection between the face and the plane
        std::pair<Vector3D, Vector3D> planeIntersection(const Vector3D & planeNormal, const Vector3D & pointOnPlane);
        
    private:
        //x, y, z vertices in counter-clockwise order
        std::shared_ptr<const MeshVertex> p_v1_ = nullptr, p_v2_ = nullptr, p_v3_ = nullptr;
        //faces that share the x/y, y/z, and z/x edges
        std::shared_ptr<const MeshFace> p_face12_ = nullptr, p_face23_ = nullptr, p_face31_ = nullptr;
        
        double area_;
        Vector3D normal_;
    };
}

#endif /* Mesh_hpp */
