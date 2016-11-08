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

using namespace mapmqp;
using namespace std;

//Mesh class functions

//TODO add fault tolerance for hashing and lowest vertices

Mesh::Mesh() { }

// void Mesh::constructSTLFromMesh(string stlFilePath){
// 	ofstream file;

// 	unsigned int twoByte = 0x0000;						//filler
// 	int size = p_faces().size();						//number of faces in mesh

// 	writeLog(INFO, "converting mesh to STL file %s...", stlFilePath.c_str());

// 	file.open(stlFilePath.c_str(), ios::out | ios::binary);	// Open the file
// 	if (file.is_open()) {                               // Check that we opened successfully

// 		for(int i = 0; i < 40; i++){					//write 80 byte header. header is unused
// 			file.write(reinterpret_cast<const char *>(&twoByte), 2);
// 		}

// 		file.write(reinterpret_cast<char*>(&size),4);

// 		for (unsigned long i = 0; i < size; ++i) {		// Loop through all triangles
// 			Vector3D normal = p_faces_[i]->p_normal();
// 			float normalX = (float)normal.x();
// 			float normalY = (float)normal.y();
// 			float normalZ = (float)normal.z();
// 			float vertex1X = (float)p_faces_[i]->p_vertex1()->vertex().x();
// 			float vertex1Y = (float)p_faces_[i]->p_vertex1()->vertex().y();
// 			float vertex1Z = (float)p_faces_[i]->p_vertex1()->vertex().z();
// 			float vertex2X = (float)p_faces_[i]->p_vertex2()->vertex().x();
// 			float vertex2Y = (float)p_faces_[i]->p_vertex2()->vertex().y();
// 			float vertex2Z = (float)p_faces_[i]->p_vertex2()->vertex().z();
// 			float vertex3X = (float)p_faces_[i]->p_vertex3()->vertex().x();
// 			float vertex3Y = (float)p_faces_[i]->p_vertex3()->vertex().y();
// 			float vertex3Z = (float)p_faces_[i]->p_vertex3()->vertex().z();

// 			file.write((char *)&normalX, 4);
// 			file.write((char *)&normalY, 4);
// 			file.write((char *)&normalZ, 4);
// 			file.write((char *)&vertex1X, 4);
// 			file.write((char *)&vertex1Y, 4);
// 			file.write((char *)&vertex1Z, 4);
// 			file.write((char *)&vertex2X, 4);
// 			file.write((char *)&vertex2Y, 4);
// 			file.write((char *)&vertex2Z, 4);
// 			file.write((char *)&vertex3X, 4);
// 			file.write((char *)&vertex3Y, 4);
// 			file.write((char *)&vertex3Z, 4);
// 			file.write((char *)&twoByte, 2);
// 		}

// 		file.close();
// 	}else {
// 		writeLog(ERROR, "unable to open file %s [errno: %d]", stlFilePath.c_str(), strerror(errno));
// 	}
// }

const vector<shared_ptr<MeshVertex>> & Mesh::p_vertices() const {
    return p_vertices_;
}

const vector<shared_ptr<MeshFace>> & Mesh::p_faces() const {
    return p_faces_;
}

void Mesh::addVertex(shared_ptr<MeshVertex> vertex) {
    p_vertices_.push_back(vertex);
}

/**
 * Adds a face to the vector of faces in the mesh
 *
 * @param face A pointer to the MeshFace to add
 */
void Mesh::addFace(shared_ptr<MeshFace> face) {
    p_faces_.push_back(face);
}

//MeshVertex class functions

MeshVertex::MeshVertex(const Vector3D & vertex) :
vertex_(vertex) { }

void MeshVertex::addConnectedFace(shared_ptr<MeshFace> p_face) {
    p_faces_.push_back(p_face);
}

Vector3D MeshVertex::vertex() const {
    return vertex_;
}

const vector<shared_ptr<const MeshFace>> & MeshVertex::p_faces() const {
    return p_faces_;
}

string MeshVertex::toString() const {
    return vertex_.toString();
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
MeshEdge::MeshEdge(shared_ptr<const MeshVertex> p_v1, shared_ptr<const MeshVertex> p_v2) {
    // All the points from each vertex
    double x1 = p_v1->vertex().x();
    double y1 = p_v1->vertex().y();
    double z1 = p_v1->vertex().z();
    
    double x2 = p_v2->vertex().x();
    double y2 = p_v2->vertex().y();
    double z2 = p_v2->vertex().z();
    
    // Check to see which is v1 and which is v2
    if (z1 < z2) {
        p_vertices_[0] = p_v1;
        p_vertices_[1] = p_v2;
    }
    else if (z2 < z1) {
        p_vertices_[0] = p_v2;
        p_vertices_[1] = p_v1;
    }
    else if (y1 < y2) {
        p_vertices_[0] = p_v1;
        p_vertices_[1] = p_v2;
    }
    else if (y2 < y1) {
        p_vertices_[0] = p_v2;
        p_vertices_[1] = p_v1;
    }
    else if (x1 < x2) {
        p_vertices_[0] = p_v1;
        p_vertices_[1] = p_v2;
    }
    else if (x2 < x1) {
        p_vertices_[0] = p_v2;
        p_vertices_[1] = p_v1;
    } else {
        throw invalid_argument("The two vertices being used to create a MeshEdge are the same");
    }
}

/**
 * Returns one of the two vertices of the edge. Takes value
 * 0 or 1 to retrieve the first and second vertex, respectively.
 * Vertices are ordered based on their lowest z-value, unless z
 * is equal in which case it uses y-value, down to x-value.
 *
 * @param v 0 or 1 depending on which vertex is desired
 *
 * @return The vertex with the lowest z-, y-, or x-value
 */
const shared_ptr<const MeshVertex> MeshEdge::p_vertex(uint16_t v) const {
    if (v < 0 || v > 1) {
        writeLog(ERROR, "tried to access vertex %d in a MeshEdge (range 0 to 1)", v);
        return nullptr;
    }
    
    return p_vertices_[v];
}

/**
 * Overrides the == operator and checks to see if the first
 * and second vectors of the current MeshEdge are equal
 * to each other.
 *
 * @return True if the MeshEdge objects have the same vertex values
 */
bool MeshEdge::operator==(const MeshEdge & edge) const {
    return p_vertices_[0]->vertex() == edge.p_vertex(0)->vertex() && p_vertices_[1]->vertex() == edge.p_vertex(1)->vertex();
}

string MeshEdge::toString() const {
    ostringstream stream;
    stream << "[" << p_vertices_[0]->toString() << ", " << p_vertices_[1]->toString() << "]";
    return stream.str();
}

//MeshFace class functions

/**
 * Builds a MeshFace from three MeshVertex objects. It is assumed that the MeshVertex
 * objects are given in counter-clockwise order, such that the normal of the
 * MeshFace can be computed using right-hand rule.
 *
 * @param v1 The first vertex
 * @param v2 The second vertex, counter-clockwise from the first
 * @param v3 The third and final vertex
 */
MeshFace::MeshFace(shared_ptr<const MeshVertex> v1, shared_ptr<const MeshVertex> v2, shared_ptr<const MeshVertex> v3) {
    // Set the vertices array
    p_vertices_[0] = v1;
    p_vertices_[1] = v2;
    p_vertices_[2] = v3;
    
    //take cross product of (y - z) and (y - z)
    Vector3D normalUnnormalized = Vector3D::crossProduct(p_vertices_[1]->vertex() - p_vertices_[0]->vertex(), p_vertices_[2]->vertex() - p_vertices_[0]->vertex());
    //area is equal to half the magnitude of a cross product
    area_ = normalUnnormalized.magnitude() / 2;
    //normalize normal
    normal_ = normalUnnormalized;
    normal_.normalize();
}

/**
 * Overrides the == operator and checks to see if the three vertices
 * of the first MeshFace are equal in value to the three vertices
 * on the given MeshFace.
 *
 * @return True if the MeshFace objects have the same vertex values
 */
bool MeshFace::operator==(const MeshFace & face) const {
    return p_vertices_[0] == face.p_vertex(0) && p_vertices_[1] == face.p_vertex(1) && p_vertices_[2] == face.p_vertex(2);
}

/**
 * Retrieves one of the 3 vertices of the face. Accepts a
 * 16-bit int between 0 and 2 inclusive. Vertices are
 * ordered counter-clockwise, so 0 -> 1 -> 2 -> 0 is
 * a counter-clockwise path around the face.
 * A nullptr is returned if an invalid vertex value
 * is requested.
 *
 * @param v The vertex to retrieve (0, 1, or 2)
 *
 * @return The requested MeshVertex pointer
 */
const shared_ptr<const MeshVertex> MeshFace::p_vertex(uint16_t v) const {
    if (v < 0 || v > 2) {
        writeLog(ERROR, "tried to access vertex %d in triangle (range 0-2)", v);
        return nullptr;
    }
    return p_vertices_[v];
}

/**
 * Retrieves one of the 3 vertices of the face. The enum values
 * FIRST, SECOND, and THIRD are provided, which follow the
 * right-hand rule.
 *
 * @param f The face to retrieve (0, 1, or 2)
 *
 * @return The requested MeshFace pointer
 */
const shared_ptr<const MeshFace> MeshFace::p_connectedFace(uint16_t f) const {
    if (f < 0 || f > 2) {
        writeLog(ERROR, "tried to access face %d in triangle (range 0-2)", f);
        return nullptr;
    }
    return p_faces_[f];
}

/**
 * Takes a face and adds it to this face at the index specified by the second
 * parameter. If an invalid edge index is given, this function does nothing.
 *
 * @param connectingFace The MeshFace to connect
 * @param edgeIndex Value between 0 and 2 specifying which edge the face to connect to is on
 */
void MeshFace::connect(shared_ptr<MeshFace> connectingFace, uint16_t edgeIndex) {
    // Write warning and return if invalid edgeIndex
    if (edgeIndex < 0 || edgeIndex > 2) {
        writeLog(WARNING, "tried to connect a face to edge index %d (range 0-2)", edgeIndex);
        return;
    }
    
    // Add this face to the given face's list of connected faces
    p_faces_[edgeIndex] = connectingFace;
}

/**
 * Takes in 2 vertices and returns the index of the edge on the face
 *
 * @param edgeVertices[] An array of 2 vertices on the face
 *
 * @return A signed int between 0 and 2 if valid edge vertices, -1 if invalid
 */
int16_t MeshFace::getEdgeIndex(shared_ptr<MeshEdge> p_edge) {
    // Find the index the given face belongs in according to edge order,
    // and then add the given face to this face's list of connected faces
    for (uint16_t i = 0; i < 3; ++i) {
        shared_ptr<const MeshVertex> thisVert1 = p_vertices_[i];
        shared_ptr<const MeshVertex> thisVert2 = p_vertices_[(i + 1) % 3];
        
        uint16_t numMatchVertices = (p_edge->p_vertex(0) == thisVert1) +\
        (p_edge->p_vertex(1) == thisVert1) +\
        (p_edge->p_vertex(0) == thisVert2) +\
        (p_edge->p_vertex(1) == thisVert2);
        if (numMatchVertices == 2) {
            return i;
        }
    }
    
    return -1;
}

bool MeshFace::intersectsPlane(const Plane & plane) const {
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(p_vertices_[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(p_vertices_[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(p_vertices_[2]->vertex());
    
    if ((vertexPos[0] == Plane::ON) || (vertexPos[1] == Plane::ON) || (vertexPos[2] == Plane::ON)) { //if any point lies exactly on plane, face intersects
        return true;
    } else if ((vertexPos[0] == Plane::BELOW) && (vertexPos[1] == Plane::BELOW) && (vertexPos[2] == Plane::BELOW)) { //if all points lie below plane, face does not intersect
        return false;
    } else if ((vertexPos[0] == Plane::ABOVE) && (vertexPos[1] == Plane::ABOVE) && (vertexPos[2] == Plane::ABOVE)) { //if all points lie above plane, face does not intersect
        return false;
    } else { //any other possibility means face intersects
        return true;
    }
}

Vector3D MeshFace::p_normal(){
    return normal_;
}

bool MeshFace::liesOnPlane(const Plane & plane) const {
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(p_vertices_[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(p_vertices_[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(p_vertices_[2]->vertex());
    
    if ((vertexPos[0] == Plane::ON) && (vertexPos[1] == Plane::ON) && (vertexPos[2] == Plane::ON)) { //if any point lies exactly on plane, face intersects
        return true;
    } else {
        return false;
    }
}

pair<Vector3D, Vector3D> MeshFace::planeIntersection(const Plane & plane) const {
    if (!intersectsPlane(plane)) {
        writeLog(WARNING, "attempted to find intersection line of MeshFace with plane that does not intersect");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    } else if (liesOnPlane(plane)) {
        writeLog(WARNING, "attempted to find intersection line of MeshFace with plane that is parallel to face");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    }
    
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(p_vertices_[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(p_vertices_[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(p_vertices_[2]->vertex());
    
    //check if any vertices lie on plane
    if (vertexPos[0] == Plane::ON) { //first vertex lies on plane
        if (vertexPos[1] == Plane::ON) { //second vertex lies on plane
            //order of vertices depends in third vertex is above or below plane
            if (vertexPos[2] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(p_vertices_[0]->vertex(), p_vertices_[1]->vertex());
            } else { //vertexPos[2] cannot be on plane
                return pair<Vector3D, Vector3D>(p_vertices_[1]->vertex(), p_vertices_[0]->vertex());
            }
        } else if (vertexPos[2] == Plane::ABOVE) {
            if (vertexPos[1] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(p_vertices_[2]->vertex(), p_vertices_[0]->vertex());
            } else {
                return pair<Vector3D, Vector3D>(p_vertices_[0]->vertex(), p_vertices_[2]->vertex());
            }
        } else {
            return pair<Vector3D, Vector3D>(p_vertices_[0]->vertex(), p_vertices_[0]->vertex());
        }
    } else if (vertexPos[1] == Plane::ON) {
        //vertexPos[0] cannot be on plane
        if (vertexPos[2] == Plane::ON) {
            if (vertexPos[0] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(p_vertices_[1]->vertex(), p_vertices_[2]->vertex());
            } else {
                return pair<Vector3D, Vector3D>(p_vertices_[2]->vertex(), p_vertices_[1]->vertex());
            }
        } else {
            return pair<Vector3D, Vector3D>(p_vertices_[1]->vertex(), p_vertices_[1]->vertex());
        }
    } else if (vertexPos[2] == Plane::ON) {
        return pair<Vector3D, Vector3D>(p_vertices_[2]->vertex(), p_vertices_[1]->vertex());
    }
    
    //no vertices lie on plane
    
    Vector3D p0, p1, p2; //p0 is on opposite side of plane as p1 and p2
    Plane::PLANE_POSITION p0Pos;
    
    //set p0 to whichever vertex is on opposite side as other vertices
    if (vertexPos[1] == vertexPos[2]) { //v0 is on side by itself
        p0 = p_vertex(0)->vertex();
        p1 = p_vertex(1)->vertex();
        p2 = p_vertex(2)->vertex();
        p0Pos = vertexPos[0];
    } else if (vertexPos[0] == vertexPos[2]) { //v1 is on side by itself
        p0 = p_vertex(1)->vertex();
        p1 = p_vertex(2)->vertex();
        p2 = p_vertex(0)->vertex();
        p0Pos = vertexPos[1];
    } else if (vertexPos[0] == vertexPos[1]) { //v2 is on side by itself
        p0 = p_vertex(2)->vertex();
        p1 = p_vertex(0)->vertex();
        p2 = p_vertex(1)->vertex();
        p0Pos = vertexPos[2];
    }
    
    Vector3D p01 = p1 - p0;
    Vector3D p02 = p2 - p0;
    
    //determine intersection using method from http://geomalgorithms.com/a05-_intersect-1.html
    
    double t01 = Vector3D::dotProduct(plane.normal(), plane.origin() - p0) / Vector3D::dotProduct(plane.normal(), p01);
    double t02 = Vector3D::dotProduct(plane.normal(), plane.origin() - p0) / Vector3D::dotProduct(plane.normal(), p02);
    
    //error checking
    if (t01 > p01.magnitude()) {
        writeLog(ERROR, "first intersection point of MeshFace edge and plane is not contained in edge");
    }
    if (t02 > p02.magnitude()) {
        writeLog(ERROR, "second intersection point of MeshFace edge and plane is not contained in edge");
    }
    
    //set intersection points to edge vectors with length of t1/t2
    Vector3D intersect01 = p01, intersect02 = p02;
    intersect01 = p0 + (intersect01 * t01);
    intersect02 = p0 + (intersect02 * t02);
    
    if (p0Pos == Plane::ABOVE) {
        return pair<Vector3D, Vector3D>(intersect01, intersect02);
    } else {
        return pair<Vector3D, Vector3D>(intersect02, intersect01);
    }
}

string MeshFace::toString() const {
    ostringstream stream;
    stream << "[" << p_vertices_[0]->toString() << ", " << p_vertices_[1]->toString() << ", " << p_vertices_[2]->toString() << "]";
    return stream.str();
}
