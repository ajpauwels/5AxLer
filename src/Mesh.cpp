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
#include <math.h>

#include "Utility.hpp"

using namespace std;
using namespace mapmqp;

/**
 * Creates a hash value for a MeshVertex using the vertex's vector
 */
struct Vector3DHash {
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

/**
 * Creates a hash value for a MeshEdge object using its vertices
 */
struct MeshEdgeHash {
    size_t operator()(const MeshEdge & me) const {
        Vector3DHash v3hasher;

        int hash = 17;
        hash = hash * 31 + v3hasher(me.getV1()->vertex());
        hash = hash * 31 + v3hasher(me.getV2()->vertex());
        return std::hash<int>()(hash);
    }
};

/**
 * Evaluates whether to MeshEdge objects are equal
 */
struct MeshEdgeEqual {
    bool operator()(const MeshEdge & me1, const MeshEdge & me2) const {
        return me1.getV1() == me2.getV1() && me1.getV2() == me2.getV2();
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
        
        unordered_map<Vector3D, shared_ptr<MeshVertex>, Vector3DHash> mappedVertices;
        unordered_map<MeshEdge, shared_ptr<MeshFace>, MeshEdgeHash, MeshEdgeEqual> mappedEdges;
        double lowestZVal = INFINITY;
        
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
                pair<unordered_map<Vector3D, shared_ptr<MeshVertex>, Vector3DHash>::iterator, bool> emplacePair = mappedVertices.emplace(vertices[i], shared_ptr<MeshVertex>(new MeshVertex(vertices[i]))); //place MeshVertex ptr into hashtable
                p_meshVertices[i] = emplacePair.first->second; //set MeshVertex ptr to returned value from hashtable in case it has changed
                if (emplacePair.second) { //if MeshVertex did not exist in hashtable, add to list of vertices
                    vertices_.push_back(p_meshVertices[i]);
                }
                
                // If the lowestVertex was never set or the current vertex is lower than the lowestVertex, replace
                // lowestVertex with the current vertex
                if (p_meshVertices[i]->vertex().z() == lowestZVal) {
                    lowestVertices_.push_back(p_meshVertices[i]);
                }
                // If a lower vertex was encountered, reset the lowest vertices list
                else if (p_meshVertices[i]->vertex().z() < lowestZVal) {
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

const vector<shared_ptr<MeshFace>> & MeshVertex::p_faces() const {
    return p_faces_;
}

//MeshEdge class functions

/**
 * The MeshEdge constructor accepts the two points representing the edge
 * as its arguments. It organizes itself into point 1 and point 2, where
 * point 1 is defined as being the point which has a lesser z-value. If it
 * has an equal value, point 1 is the lesser y-value, and if those are equal,
 * the lesser x-value. If the points are equal, MeshEdge throws an invalid_argument
 * exception.
 * This makes it possible to create two MeshEdge objects with the same points
 * but given in any order, and the MeshEdge objects will always be equivalent.
 *
 * @params v1 One of the vertices on the edge
 * @params v2 The other vertex on the edge
 */
MeshEdge::MeshEdge(shared_ptr<MeshVertex> v1, shared_ptr<MeshVertex> v2) {
    // All the points from each vertex
    double x1 = v1->vertex().x();
    double y1 = v1->vertex().y();
    double z1 = v1->vertex().z();

    double x2 = v2->vertex().x();
    double y2 = v2->vertex().y();
    double z2 = v2->vertex().z();

    // Check to see which is v1 and which is v2
    if (z1 < z2) {
        v1_ = v1;
        v2_ = v2;
    }
    else if (z2 < z1) {
        v1_ = v2;
        v2_ = v1;
    }
    else if (y1 < y2) {
        v1_ = v1;
        v2_ = v2;
    }
    else if (y2 < y1) {
        v1_ = v2;
        v2_ = v1;
    }
    else if (x1 < x2) {
        v1_ = v1;
        v2_ = v2;
    }
    else if (x2 < x1) {
        v1_ = v2;
        v2_ = v1;
    } else {
        throw std::invalid_argument("The two vertices being used to create a MeshEdge are the same");
    }
}

std::shared_ptr<MeshVertex> MeshEdge::getV1() const {
    return v1_;
}

std::shared_ptr<MeshVertex> MeshEdge::getV2() const {
    return v2_;
}

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
    double v1Val = Vector3D::dotProduct(planeNormal, v1_->vertex() - pointOnPlane);
    double v2Val = Vector3D::dotProduct(planeNormal, v2_->vertex() - pointOnPlane);
    double v3Val = Vector3D::dotProduct(planeNormal, v3_->vertex() - pointOnPlane);
    
    if ((v1Val == 0) || (v2Val == 0) || (v3Val == 0)) { //if any point lies exactly on plane, face intersects
        return true;
    } else if ((v1Val < 0) && (v2Val < 0) && (v3Val < 0)) { //if all points lie below plane, face does not intersect
        return false;
    } else if ((v1Val < 0) && (v2Val < 0) && (v3Val < 0)) { //if all points lie above plane, face does not intersect
        return false;
    } else { //any other possibility means face intersects
        return true;
    }
}

bool MeshFace::liesOnPlane(Vector3D planeNormal, Vector3D pointOnPlane) {
    double v1Val = Vector3D::dotProduct(planeNormal, v1_->vertex() - pointOnPlane);
    double v2Val = Vector3D::dotProduct(planeNormal, v2_->vertex() - pointOnPlane);
    double v3Val = Vector3D::dotProduct(planeNormal, v3_->vertex() - pointOnPlane);
    
    if ((v1Val == 0) && (v2Val == 0) && (v3Val == 0)) { //if every point lies exactly on plane
        return true;
    } else {
        return false;
    }
}

pair<Vector3D, Vector3D> MeshFace::planeIntersection(Vector3D planeNormal, Vector3D pointOnPlane) {
    //TODO by finding which point (A) lies on different side than other points (B and C), and finding intersections of lines containing A-B and A-C
    if (intersectsPlane(planeNormal, pointOnPlane)) {
        writeLog(WARNING_MESSAGE, "attempted to find intersection line of MeshFace with plane that does not intersect");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    } else if (liesOnPlane(planeNormal, pointOnPlane)) {
        writeLog(WARNING_MESSAGE, "attempted to find intersection line of MeshFace with plane that is parallel to face");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    }
    
    double v1Val = Vector3D::dotProduct(planeNormal, v1_->vertex() - pointOnPlane);
    double v2Val = Vector3D::dotProduct(planeNormal, v2_->vertex() - pointOnPlane);
    double v3Val = Vector3D::dotProduct(planeNormal, v3_->vertex() - pointOnPlane);
    
    //if two vertices lie of plane, return edge between vertices
    //TODO determine which order vertices should be placed in pair
    if ((v1Val == 0) && (v2Val == 0)) {
        return pair<Vector3D, Vector3D>(v1_->vertex(), v2_->vertex());
    } else if ((v1Val == 0) && (v3Val == 0)) {
        return pair<Vector3D, Vector3D>(v1_->vertex(), v3_->vertex());
    } else if ((v2Val == 0) && (v3Val == 0)) {
        return pair<Vector3D, Vector3D>(v2_->vertex(), v3_->vertex());
    }
    
    //using method described in stackoverflow post: http://math.stackexchange.com/questions/100439/determine-where-a-vector-will-intersect-a-plane
    
    Vector3D p0, p1, p2; //p0 is on opposite side of plane as p1 and p2
    //set p0 to whichever vertex is on opposite side as other vertices
    if (((v1Val <= 0) && (v2Val > 0) && (v3Val > 0)) || ((v1Val >= 0) && (v2Val < 0) && (v3Val < 0))) { //v1 is on side by itself
        p0 = v1_->vertex();
        p1 = v2_->vertex();
        p2 = v3_->vertex();
    } else if (((v2Val <= 0) && (v1Val > 0) && (v3Val > 0)) || ((v2Val >= 0) && (v1Val < 0) && (v3Val < 0))) { //v2 is on side by itself
        p0 = v2_->vertex();
        p1 = v1_->vertex();
        p2 = v3_->vertex();
    } else if (((v3Val <= 0) && (v1Val > 0) && (v2Val > 0)) || ((v3Val >= 0) && (v1Val < 0) && (v2Val < 0))) { //v3 is on side by itself
        p0 = v3_->vertex();
        p1 = v1_->vertex();
        p2 = v2_->vertex();
    }
    
    Vector3D p01 = p1 - p0;
    Vector3D p02 = p2 - p0;
    
    double t1 = Vector3D::dotProduct(planeNormal, pointOnPlane - p0) / Vector3D::dotProduct(planeNormal, p01);
    double t2 = Vector3D::dotProduct(planeNormal, pointOnPlane - p0) / Vector3D::dotProduct(planeNormal, p02);
    
    //error checking
    if (t1 > p01.magnitude()) {
        writeLog(ERROR_MESSAGE, "first intersection point of MeshFace edge and plane is not contained in edge");
    }
    if (t2 > p01.magnitude()) {
        writeLog(ERROR_MESSAGE, "second intersection point of MeshFace edge and plane is not contained in edge");
    }
    
    //set intersection points to edge vectors with length of t1/t2
    Vector3D intersect1 = p01, intersect2 = p02;
    intersect1.normalize(t1);
    intersect2.normalize(t2);
    
    return pair<Vector3D, Vector3D>(intersect1, intersect2);
}
