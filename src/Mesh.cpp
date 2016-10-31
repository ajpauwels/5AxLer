//
//  Mesh.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 10/30/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Mesh.hpp"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "Utility.hpp"

using namespace std;
using namespace mapmqp;

//helper struct for unordered map used in parsing the STL file
struct MeshVertexHash {
    size_t operator()(const Vector3D & v) const { //TODO use sizeof(long) instead? should they be longs or ints
        long x = (long)v.x();
        long y = (long)v.y();
        long z = (long)v.z();
        
        int hashVal = (int)(x ^ (x >> 32));
        hashVal = 31 * hashVal + (int)(y ^ (y >> 32));
        hashVal = 31 * hashVal + (int)(z ^ (z >> 32));
        return hash<int>()(hashVal);
    }
};

struct MeshVertexEqual {
    bool operator()(const Vector3D & v1, const Vector3D & v2) const {
        return v1.x() == v2.x() && v1.y() == v2.y() && v1.z() == v2.z();
    }
};

//Mesh class functions

//TODO add fault tolerance for hashing and lowest vertices

Mesh::Mesh() { }

void Mesh::constructMeshFromSTL(string stlFilePath) {
    ifstream file;									// Our file handler
    char *header = new char[80];					// The 80-char file header
    unsigned int size;								// The number of triangles in the file
    
    writeLog(INFO_MESSAGE, "parsing STL file %s...", stlFilePath.c_str());
    file.open(stlFilePath.c_str(), ios::in | ios::binary);	// Open the file
    if (file.is_open()) {                               // Check that we opened successfully
        file.read(header, 80);							// Get the header
        file.read((char*)&size, 4);						// Get the number of triangles
        
        unordered_map<Vector3D, shared_ptr<MeshVertex>, MeshVertexHash, MeshVertexEqual> mappedVertices;
        
        double lowestZVal = INFINITY; //TODO should this be an int?
        
        for (unsigned int i = 0; i < size; ++i) {		// Loop through all triangles
            Vector3D norm, vertices[3];                 // Stores the three triangle points + normal vector
            float points[12] = { };						// 4 vectors * 3 points = 12 points
            short abc;									// Stores the attribute byte count
            
            for (unsigned int j = 0; j < 12; ++j) {		// Get all points from file
                file.read((char*)(points + j), 4);
            }
            file.read((char*)&abc, 2);
            
            norm = Vector3D(points[0], points[1], points[2]);           // Create normal vector
            vertices[0] = Vector3D(points[3], points[4], points[5]);	// Get first point of triangle
            vertices[1] = Vector3D(points[6], points[7], points[8]);	// Get second point of triangle
            vertices[2] = Vector3D(points[9], points[10], points[11]);	// Get third point of triangle
            
            shared_ptr<MeshVertex> p_meshVertices[3]; //Three MeshVertex pointers
            
            for (unsigned int i = 0; i < 3; i++) {
                pair<unordered_map<Vector3D, shared_ptr<MeshVertex>, MeshVertexHash, MeshVertexEqual>::iterator, bool> emplacePair = mappedVertices.emplace(vertices[i], shared_ptr<MeshVertex>(new MeshVertex(vertices[i]))); //place MeshVertex ptr into hashtable
                p_meshVertices[i] = emplacePair.first->second; //set MeshVertex ptr to returned value from hashtable in case it has changed
                if (emplacePair.second) { //if MeshVertex did not exist in hashtable, add to list of vertices
                    vertices_.push_back(p_meshVertices[i]);
                }
                
                //check if any vertices are a lowest vertex
                if (p_meshVertices[i]->vertex().z() == lowestZVal) { //vertex is a lowest vertex
                    lowestVertices_.push_back(p_meshVertices[i]);
                } else if (p_meshVertices[i]->vertex().z() < lowestZVal) { //vertex is lower than other lowest vertices
                    //reset list of lowest vertices
                    lowestZVal = p_meshVertices[i]->vertex().z();
                    lowestVertices_.clear();
                    lowestVertices_.push_back(p_meshVertices[i]);
                }
            }
            
            shared_ptr<MeshFace> p_meshFace(new MeshFace(p_meshVertices[0], p_meshVertices[1], p_meshVertices[2]));
            faces_.push_back(p_meshFace);
            //add p_meshFace to all list of connected faces in vertices
            for (unsigned int i = 0; i < 3; i++) {
                p_meshVertices[i]->p_faces_.push_back(p_meshFace);
            }
            
            //TODO add adjacent faces
        }
        file.close();	// Close the file
    } else {
        writeLog(ERROR_MESSAGE, "unable to open file %s [errno: %d]", stlFilePath.c_str(), strerror(errno));
    }
}

const vector<shared_ptr<MeshVertex>> & Mesh::vertices() {
    return vertices_;
}

const vector<shared_ptr<MeshFace>> & Mesh::faces() {
    return faces_;
}

//MeshVertex class functions

MeshVertex::MeshVertex(Vector3D vertex) :
vertex_(vertex) { }

Vector3D MeshVertex::vertex() const {
    return vertex_;
}

const vector<const shared_ptr<MeshFace>> & MeshVertex::p_faces() const {
    return p_faces_;
}

//MeshEdge class functions

MeshEdge::MeshEdge(shared_ptr<MeshVertex> v1, shared_ptr<MeshVertex> v2) :
v1_(v1),
v2_(v2) { }

//MeshFace class functions

MeshFace::MeshFace(shared_ptr<MeshVertex> v1, shared_ptr<MeshVertex> v2, shared_ptr<MeshVertex> v3) :
v1_(v1),
v2_(v2),
v3_(v3) {
    //take cross product of (y - z) and (y - z)
    Vector3D normalUnnormalized = Vector3D::crossProduct(v2->vertex() - v1->vertex(), v3->vertex() - v1->vertex());
    //area is equal to half the magnitude of a cross product
    area_ = normalUnnormalized.magnitude() / 2;
    //normalize normal
    normal_ = normalUnnormalized;
    normal_.normalize();
}

shared_ptr<MeshVertex> MeshFace::v1() {
    return v1_;
}

shared_ptr<MeshVertex> MeshFace::v2() {
    return v2_;
}

shared_ptr<MeshVertex> MeshFace::v3() {
    return v3_;
}

shared_ptr<MeshFace> MeshFace::face12() {
    return face12_;
}

shared_ptr<MeshFace> MeshFace::face23() {
    return face23_;
}

shared_ptr<MeshFace> MeshFace::face31() {
    return face31_;
}

bool MeshFace::intersectsPlane(Vector3D planeNormal, Vector3D pointOnPlane) {
    //TODO by checking if one vertex lies of a different side of the plane than the other two (or any point lies on the plane)
    return false;
}

bool MeshFace::liesOnPlane(Vector3D planeNormal, Vector3D pointOnPlane) {
    //TODO by checking whether or not every vertex lies on the plane
    return false;
}

pair<Vector3D, Vector3D> MeshFace::planeIntersection(Vector3D planeNormal, Vector3D pointOnPlane) {
    //TODO by finding which point (A) lies on different side than other points (B and C), and finding intersections of lines containing A-B and A-C
    return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
}
