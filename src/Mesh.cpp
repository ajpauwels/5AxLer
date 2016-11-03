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

void Mesh::constructSTLFromMesh(string stlFilePath){
	ofstream file;
	
	unsigned int twoByte = 0x0000;						//filler
	int size = p_faces().size();						//number of faces in mesh
	
	writeLog(INFO, "converting mesh to STL file %s...", stlFilePath.c_str());
	
	file.open(stlFilePath.c_str(), ios::out | ios::binary);	// Open the file
	if (file.is_open()) {                               // Check that we opened successfully
		
		for(int i = 0; i < 40; i++){					//write 80 byte header. header is unused
			file.write(reinterpret_cast<const char *>(&twoByte), 2);
		}
		
		file.write(reinterpret_cast<char*>(&size),4);
		
		for (unsigned long i = 0; i < size; ++i) {		// Loop through all triangles
			Vector3D normal = p_faces_[i]->p_normal();
			float normalX = (float)normal.x();
			float normalY = (float)normal.y();
			float normalZ = (float)normal.z();
			float vertex1X = (float)p_faces_[i]->p_vertex1()->vertex().x();
			float vertex1Y = (float)p_faces_[i]->p_vertex1()->vertex().y();
			float vertex1Z = (float)p_faces_[i]->p_vertex1()->vertex().z();
			float vertex2X = (float)p_faces_[i]->p_vertex2()->vertex().x();
			float vertex2Y = (float)p_faces_[i]->p_vertex2()->vertex().y();
			float vertex2Z = (float)p_faces_[i]->p_vertex2()->vertex().z();
			float vertex3X = (float)p_faces_[i]->p_vertex3()->vertex().x();
			float vertex3Y = (float)p_faces_[i]->p_vertex3()->vertex().y();
			float vertex3Z = (float)p_faces_[i]->p_vertex3()->vertex().z();
			
			file.write((char *)&normalX, 4);
			file.write((char *)&normalY, 4);
			file.write((char *)&normalZ, 4);
			file.write((char *)&vertex1X, 4);
			file.write((char *)&vertex1Y, 4);
			file.write((char *)&vertex1Z, 4);
			file.write((char *)&vertex2X, 4);
			file.write((char *)&vertex2Y, 4);
			file.write((char *)&vertex2Z, 4);
			file.write((char *)&vertex3X, 4);
			file.write((char *)&vertex3Y, 4);
			file.write((char *)&vertex3Z, 4);
			file.write((char *)&twoByte, 2);
		}
		
		file.close();
	} else {
		writeLog(ERROR, "unable to open file %s [errno: %d]", stlFilePath.c_str(), strerror(errno));
	}
}

void Mesh::constructMeshFromSTL(string stlFilePath) {
    ifstream file;									// Our file handler
    char *header = new char[80];					// The 80-char file header
    unsigned int size;								// The number of triangles in the file
    
    writeLog(INFO, "parsing STL file %s...", stlFilePath.c_str());
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
                    p_vertices_.push_back(p_meshVertices[i]);
                }
                
                //check if any vertices are a lowest vertex
                if (p_meshVertices[i]->vertex().z() == lowestZVal) { //vertex is a lowest vertex
                    p_lowestVertices_.push_back(p_meshVertices[i]);
                } else if (p_meshVertices[i]->vertex().z() < lowestZVal) { //vertex is lower than other lowest vertices
                    //reset list of lowest vertices
                    lowestZVal = p_meshVertices[i]->vertex().z();
                    p_lowestVertices_.clear();
                    p_lowestVertices_.push_back(p_meshVertices[i]);
                }
            }
            
            shared_ptr<MeshFace> p_meshFace(new MeshFace(p_meshVertices[0], p_meshVertices[1], p_meshVertices[2]));
            p_faces_.push_back(p_meshFace);
            //add p_meshFace to all list of connected faces in vertices
            for (unsigned int i = 0; i < 3; i++) {
                p_meshVertices[i]->p_faces_.push_back(p_meshFace);
            }
            
            //TODO add adjacent faces
        }
        file.close();	// Close the file
    } else {
        writeLog(ERROR, "unable to open file %s [errno: %d]", stlFilePath.c_str(), strerror(errno));
    }
}

const vector<shared_ptr<MeshVertex>> & Mesh::p_vertices() const {
    return p_vertices_;
}

const vector<shared_ptr<MeshFace>> & Mesh::p_faces() const {
    return p_faces_;
}

std::vector<Polygon> Mesh::planeIntersection(const Vector3D & planeNormal, const Vector3D & planeOrigin) const {
    vector<pair<pair<Vector3D, Vector3D>, shared_ptr<const MeshFace>>> faceIntersections;
    
    //find all intersections with plane
    for (vector<shared_ptr<MeshFace>>::const_iterator it = p_faces_.begin(); it != p_faces().end(); it++) {
        shared_ptr<MeshFace> p_face = *it;
        if (p_face->intersectsPlane(planeNormal, planeOrigin)) {
            //TODO check for face lying entirely on plane
            faceIntersections.push_back(pair<pair<Vector3D, Vector3D>, shared_ptr<MeshFace>>(p_face->planeIntersection(planeNormal, planeOrigin), *it));
        }
    }
    
    vector<Polygon> polygons;
    
    while (faceIntersections.size() > 0) {
        if (faceIntersections.size() < 3) {
            writeLog(ERROR, "attempting to create polygon from less than 3 face intersections");
            break;
        }
        
        vector<pair<pair<Vector3D, Vector3D>, shared_ptr<const MeshFace>>> polygonLines;
        polygonLines.push_back(pair<pair<Vector3D, Vector3D>, shared_ptr<const MeshFace>>(faceIntersections[0].first, faceIntersections[0].second));
        faceIntersections.erase(faceIntersections.begin());
        
        Vector3D front = polygonLines[0].first.first;
        Vector3D back = polygonLines[0].first.second;
        
        bool breakWhile = true;
        while (breakWhile) {
            breakWhile = true;
            for (vector<pair<pair<Vector3D, Vector3D>, shared_ptr<const MeshFace>>>::iterator it = faceIntersections.begin(); it != faceIntersections.end(); it++) {
                pair<Vector3D, Vector3D> line = it->first;
            }
        }
    }
    
    return polygons;
}

//MeshVertex class functions

MeshVertex::MeshVertex(const Vector3D & vertex) :
vertex_(vertex) { }

Vector3D MeshVertex::vertex() const {
    return vertex_;
}

const vector<shared_ptr<const MeshFace>> & MeshVertex::p_faces() const {
    return p_faces_;
}

//MeshEdge class functions

MeshEdge::MeshEdge(shared_ptr<const MeshVertex> p_vertex1, shared_ptr<const MeshVertex> p_vertex2) :
p_v1_(p_vertex1),
p_v2_(p_vertex2) { }

//MeshFace class functions

MeshFace::MeshFace(shared_ptr<const MeshVertex> p_vertex1, shared_ptr<const MeshVertex> p_vertex2, shared_ptr<const MeshVertex> p_vertex3) :
p_v1_(p_vertex1),
p_v2_(p_vertex2),
p_v3_(p_vertex3) {
    //take cross product of (y - z) and (y - z)
    Vector3D normalUnnormalized = Vector3D::crossProduct(p_vertex2->vertex() - p_vertex1->vertex(), p_vertex3->vertex() - p_vertex1->vertex());
    //area is equal to half the magnitude of a cross product
    area_ = normalUnnormalized.magnitude() / 2;
    //normalize normal
    normal_ = normalUnnormalized;
    normal_.normalize();
}

shared_ptr<const MeshVertex> MeshFace::p_vertex1() const {
    return p_v1_;
}

shared_ptr<const MeshVertex> MeshFace::p_vertex2() const {
    return p_v2_;
}

shared_ptr<const MeshVertex> MeshFace::p_vertex3() const {
    return p_v3_;
}

shared_ptr<const MeshFace> MeshFace::p_face12() const {
    return p_face12_;
}

shared_ptr<const MeshFace> MeshFace::p_face23() const {
    return p_face23_;
}

shared_ptr<const MeshFace> MeshFace::p_face31() const {
    return p_face31_;
}

Vector3D MeshFace::p_normal(){
	return normal_;
}

bool MeshFace::intersectsPlane(const Vector3D & planeNormal, const Vector3D & planeOrigin) const {
    double v1Val = Vector3D::dotProduct(planeNormal, p_v1_->vertex() - planeOrigin);
    double v2Val = Vector3D::dotProduct(planeNormal, p_v2_->vertex() - planeOrigin);
    double v3Val = Vector3D::dotProduct(planeNormal, p_v3_->vertex() - planeOrigin);
    
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

bool MeshFace::liesOnPlane(const Vector3D & planeNormal, const Vector3D & planeOrigin) const {
    double v1Val = Vector3D::dotProduct(planeNormal, p_v1_->vertex() - planeOrigin);
    double v2Val = Vector3D::dotProduct(planeNormal, p_v2_->vertex() - planeOrigin);
    double v3Val = Vector3D::dotProduct(planeNormal, p_v3_->vertex() - planeOrigin);
    
    if ((v1Val == 0) && (v2Val == 0) && (v3Val == 0)) { //if every point lies exactly on plane
        return true;
    } else {
        return false;
    }
}

pair<Vector3D, Vector3D> MeshFace::planeIntersection(const Vector3D & planeNormal, const Vector3D & planeOrigin) const {
    //TODO by finding which point (A) lies on different side than other points (B and C), and finding intersections of lines containing A-B and A-C
    if (intersectsPlane(planeNormal, planeOrigin)) {
        writeLog(WARNING, "attempted to find intersection line of MeshFace with plane that does not intersect");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    } else if (liesOnPlane(planeNormal, planeOrigin)) {
        writeLog(WARNING, "attempted to find intersection line of MeshFace with plane that is parallel to face");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    }
    
    double v1Val = Vector3D::dotProduct(planeNormal, p_v1_->vertex() - planeOrigin);
    double v2Val = Vector3D::dotProduct(planeNormal, p_v2_->vertex() - planeOrigin);
    double v3Val = Vector3D::dotProduct(planeNormal, p_v3_->vertex() - planeOrigin);
    
    //if two vertices lie of plane, return edge between vertices
    //TODO determine which order vertices should be placed in pair
    if ((v1Val == 0) && (v2Val == 0)) {
        return pair<Vector3D, Vector3D>(p_v1_->vertex(), p_v2_->vertex());
    } else if ((v1Val == 0) && (v3Val == 0)) {
        return pair<Vector3D, Vector3D>(p_v1_->vertex(), p_v3_->vertex());
    } else if ((v2Val == 0) && (v3Val == 0)) {
        return pair<Vector3D, Vector3D>(p_v2_->vertex(), p_v3_->vertex());
    }
    
    //using method described in stackoverflow post: http://math.stackexchange.com/questions/100439/determine-where-a-vector-will-intersect-a-plane
    
    Vector3D p0, p1, p2; //p0 is on opposite side of plane as p1 and p2
    //set p0 to whichever vertex is on opposite side as other vertices
    if (((v1Val <= 0) && (v2Val > 0) && (v3Val > 0)) || ((v1Val >= 0) && (v2Val < 0) && (v3Val < 0))) { //v1 is on side by itself
        p0 = p_v1_->vertex();
        p1 = p_v2_->vertex();
        p2 = p_v3_->vertex();
    } else if (((v2Val <= 0) && (v1Val > 0) && (v3Val > 0)) || ((v2Val >= 0) && (v1Val < 0) && (v3Val < 0))) { //v2 is on side by itself
        p0 = p_v2_->vertex();
        p1 = p_v1_->vertex();
        p2 = p_v3_->vertex();
    } else if (((v3Val <= 0) && (v1Val > 0) && (v2Val > 0)) || ((v3Val >= 0) && (v1Val < 0) && (v2Val < 0))) { //v3 is on side by itself
        p0 = p_v3_->vertex();
        p1 = p_v1_->vertex();
        p2 = p_v2_->vertex();
    }
    
    Vector3D p01 = p1 - p0;
    Vector3D p02 = p2 - p0;
    
    double t01 = Vector3D::dotProduct(planeNormal, planeOrigin - p0) / Vector3D::dotProduct(planeNormal, p01);
    double t02 = Vector3D::dotProduct(planeNormal, planeOrigin - p0) / Vector3D::dotProduct(planeNormal, p02);
    
    //error checking
    if (t01 > p01.magnitude()) {
        writeLog(ERROR, "first intersection point of MeshFace edge and plane is not contained in edge");
    }
    if (t02 > p01.magnitude()) {
        writeLog(ERROR, "second intersection point of MeshFace edge and plane is not contained in edge");
    }
    
    //set intersection points to edge vectors with length of t1/t2
    Vector3D intersect01 = p01, intersect02 = p02;
    intersect01.normalize(t01);
    intersect02.normalize(t02);
    
    //TODO order these
    
    return pair<Vector3D, Vector3D>(intersect01, intersect02);
}
