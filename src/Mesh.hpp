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
        
        void constructMeshFromSTL(std::string stlFilePath);
        
        const std::vector<std::shared_ptr<MeshVertex>> & vertices();
        const std::vector<std::shared_ptr<MeshFace>> & faces();
        
    private:
        std::vector<std::shared_ptr<MeshVertex>> vertices_;
        std::vector<std::shared_ptr<MeshFace>> faces_;
        
        std::vector<std::shared_ptr<MeshVertex>> lowestVertices_;
    };
    
    class MeshVertex {
        friend class Mesh;
    public:
        MeshVertex(Vector3D vertex);
        
        //getters
        Vector3D vertex() const;
        const std::vector<const std::shared_ptr<MeshFace>> & p_faces() const;
        
    private:
        Vector3D vertex_;
        std::vector<const std::shared_ptr<MeshFace>> p_faces_; //all faces that have this vertex as a vertex
    };
    
    class MeshEdge {
        friend class Mesh;
    public:
        MeshEdge(std::shared_ptr<MeshVertex> v1, std::shared_ptr<MeshVertex> v2);
        
    private:
        std::shared_ptr<MeshVertex> v1_ = nullptr, v2_ = nullptr;
    };
    
    class MeshFace {
        friend class Mesh;
    public:
        MeshFace(std::shared_ptr<MeshVertex> v1, std::shared_ptr<MeshVertex> v2, std::shared_ptr<MeshVertex> v3); //TODO add normal checking
        
        //getters
        std::shared_ptr<MeshVertex> v1();
        std::shared_ptr<MeshVertex> v2();
        std::shared_ptr<MeshVertex> v3();
        std::shared_ptr<MeshFace> face12();
        std::shared_ptr<MeshFace> face23();
        std::shared_ptr<MeshFace> face31();
        
    private:
        //x, y, z vertices in counter-clockwise order
        std::shared_ptr<MeshVertex> v1_ = nullptr, v2_ = nullptr, v3_ = nullptr;
        //faces that share the x/y, y/z, and z/x edges
        std::shared_ptr<MeshFace> face12_ = nullptr, face23_ = nullptr, face31_ = nullptr;
        
        double area_;
        Vector3D normal_;
    };
}

#endif /* Mesh_hpp */
