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

//TODO add fault tolerance for hashing and lowest vertices - this might already be covered with the initial rounding?

Mesh::Mesh() { }

const vector<shared_ptr<Mesh::Vertex>> & Mesh::p_vertices() const {
    return m_p_vertices;
}

const vector<shared_ptr<Mesh::Face>> & Mesh::p_faces() const {
    return m_p_faces;
}

void Mesh::addVertex(shared_ptr<Mesh::Vertex> vertex) {
    m_p_vertices.push_back(vertex);
}

/**
 * Applies a transformation to every Vertex in the Mesh
 *
 * @param function pointer that returns void and takes in a reference to a Vector3D and applies transformation to the Vector3D
 */
void Mesh::transform(void (*transformFnc)(Vector3D & v)) {
    //TODO reset and rebuild lowest vertices
    
    for (vector<shared_ptr<Mesh::Vertex>>::iterator it = m_p_vertices.begin(); it != m_p_vertices.end(); it++) {
        shared_ptr<Mesh::Vertex> p_vertex = *it;
        transformFnc(p_vertex->m_vertex);
    }
    
    for (vector<shared_ptr<Mesh::Face>>::iterator it = m_p_faces.begin(); it != m_p_faces.end(); it++) {
        shared_ptr<Mesh::Face> p_face = *it;
        
        //take cross product of (v1 - v0) and (v2 - v0)
        Vector3D normalUnnormalized = Vector3D::crossProduct(m_p_vertices[1]->vertex() - m_p_vertices[0]->vertex(), m_p_vertices[2]->vertex() - m_p_vertices[0]->vertex());
        //area is equal to half the magnitude of a cross product
        p_face->m_area = normalUnnormalized.magnitude() / 2;
        //normalize normal
        p_face->m_normal = normalUnnormalized;
        p_face->m_normal.normalize();
    }
}

/**
 * Adds a face to the vector of faces in the mesh
 *
 * @param face A pointer to the Mesh::Face to add
 */
void Mesh::addFace(shared_ptr<Mesh::Face> face) {
    m_p_faces.push_back(face);
}

//Mesh::Vertex class functions

Mesh::Vertex::Vertex(const Vector3D & vertex) :
m_vertex(vertex) { }

void Mesh::Vertex::addConnectedFace(shared_ptr<Mesh::Face> p_face) {
    m_p_faces.push_back(p_face);
}

Vector3D Mesh::Vertex::vertex() const {
    return m_vertex;
}

const vector<shared_ptr<const Mesh::Face>> & Mesh::Vertex::p_faces() const {
    return m_p_faces;
}

string Mesh::Vertex::toString() const {
    return m_vertex.toString();
}

//Mesh::Edge class functions

/**
 * The Mesh::Edge constructor accepts the two points representing the edge
 * as its arguments. It organizes itself into point 1 and point 2, where
 * point 1 is defined as being the point which has a lesser z-value. If it
 * has an equal value, point 1 is the lesser y-value, and if those are equal,
 * the lesser x-value. If the points are equal, Mesh::Edge throws an invalid_argument
 * exception.
 * This makes it possible to create two Mesh::Edge objects with the same points
 * but given in any order, and the Mesh::Edge objects will always be equivalent.
 *
 * @params v1 One of the vertices on the edge
 * @params v2 The other vertex on the edge
 */
Mesh::Edge::Edge(shared_ptr<const Mesh::Vertex> p_v1, shared_ptr<const Mesh::Vertex> p_v2) {
    // All the points from each vertex
    double x1 = p_v1->vertex().x();
    double y1 = p_v1->vertex().y();
    double z1 = p_v1->vertex().z();
    
    double x2 = p_v2->vertex().x();
    double y2 = p_v2->vertex().y();
    double z2 = p_v2->vertex().z();
    
    // Check to see which is v1 and which is v2
    if (z1 < z2) {
        m_p_vertices[0] = p_v1;
        m_p_vertices[1] = p_v2;
    }
    else if (z2 < z1) {
        m_p_vertices[0] = p_v2;
        m_p_vertices[1] = p_v1;
    }
    else if (y1 < y2) {
        m_p_vertices[0] = p_v1;
        m_p_vertices[1] = p_v2;
    }
    else if (y2 < y1) {
        m_p_vertices[0] = p_v2;
        m_p_vertices[1] = p_v1;
    }
    else if (x1 < x2) {
        m_p_vertices[0] = p_v1;
        m_p_vertices[1] = p_v2;
    }
    else if (x2 < x1) {
        m_p_vertices[0] = p_v2;
        m_p_vertices[1] = p_v1;
    } else {
        throw invalid_argument("The two vertices being used to create a Mesh::Edge are the same");
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
const shared_ptr<const Mesh::Vertex> Mesh::Edge::p_vertex(uint16_t v) const {
    if (v < 0 || v > 1) {
        writeLog(ERROR, "tried to access vertex %d in a Mesh::Edge (range 0 to 1)", v);
        return nullptr;
    }
    
    return m_p_vertices[v];
}

/**
 * Overrides the == operator and checks to see if the first
 * and second vectors of the current Mesh::Edge are equal
 * to each other.
 *
 * @return True if the Mesh::Edge objects have the same vertex values
 */
bool Mesh::Edge::operator==(const Mesh::Edge & edge) const {
    return m_p_vertices[0]->vertex() == edge.p_vertex(0)->vertex() && m_p_vertices[1]->vertex() == edge.p_vertex(1)->vertex();
}

string Mesh::Edge::toString() const {
    ostringstream stream;
    stream << "[" << m_p_vertices[0]->toString() << ", " << m_p_vertices[1]->toString() << "]";
    return stream.str();
}

//Mesh::Face class functions

/**
 * Builds a Mesh::Face from three Mesh::Vertex objects. It is assumed that the Mesh::Vertex
 * objects are given in counter-clockwise order, such that the normal of the
 * Mesh::Face can be computed using right-hand rule.
 *
 * @param v1 The first vertex
 * @param v2 The second vertex, counter-clockwise from the first
 * @param v3 The third and final vertex
 */
Mesh::Face::Face(shared_ptr<const Mesh::Vertex> v0, shared_ptr<const Mesh::Vertex> v1, shared_ptr<const Mesh::Vertex> v2) {
    // Set the vertices array
    m_p_vertices[0] = v0;
    m_p_vertices[1] = v1;
    m_p_vertices[2] = v2;
    
    //take cross product of (v1 - v0) and (v2 - v0)
    Vector3D normalUnnormalized = Vector3D::crossProduct(m_p_vertices[1]->vertex() - m_p_vertices[0]->vertex(), m_p_vertices[2]->vertex() - m_p_vertices[0]->vertex());
    //area is equal to half the magnitude of a cross product
    m_area = normalUnnormalized.magnitude() / 2;
    //normalize normal
    m_normal = normalUnnormalized;
    m_normal.normalize();
}

/**
 * Overrides the == operator and checks to see if the three vertices
 * of the first Mesh::Face are equal in value to the three vertices
 * on the given Mesh::Face.
 *
 * @return True if the Mesh::Face objects have the same vertex values
 */
bool Mesh::Face::operator==(const Mesh::Face & face) const {
    return m_p_vertices[0] == face.p_vertex(0) && m_p_vertices[1] == face.p_vertex(1) && m_p_vertices[2] == face.p_vertex(2);
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
 * @return The requested Mesh::Vertex pointer
 */
const shared_ptr<const Mesh::Vertex> Mesh::Face::p_vertex(uint16_t v) const {
    if (v < 0 || v > 2) {
        writeLog(ERROR, "tried to access vertex %d in triangle (range 0-2)", v);
        return nullptr;
    }
    return m_p_vertices[v];
}

/**
 * Retrieves one of the 3 vertices of the face. The enum values
 * FIRST, SECOND, and THIRD are provided, which follow the
 * right-hand rule.
 *
 * @param f The face to retrieve (0, 1, or 2)
 *
 * @return The requested Mesh::Face pointer
 */
const shared_ptr<const Mesh::Face> Mesh::Face::p_connectedFace(uint16_t f) const {
    if (f < 0 || f > 2) {
        writeLog(ERROR, "tried to access face %d in triangle (range 0-2)", f);
        return nullptr;
    }
    return m_p_faces[f];
}

/**
 * Takes a face and adds it to this face at the index specified by the second
 * parameter. If an invalid edge index is given, this function does nothing.
 *
 * @param connectingFace The Mesh::Face to connect
 * @param edgeIndex Value between 0 and 2 specifying which edge the face to connect to is on
 */
void Mesh::Face::connect(shared_ptr<Mesh::Face> connectingFace, uint16_t edgeIndex) {
    // Write warning and return if invalid edgeIndex
    if (edgeIndex < 0 || edgeIndex > 2) {
        writeLog(WARNING, "tried to connect a face to edge index %d (range 0-2)", edgeIndex);
        return;
    }
    
    // Add this face to the given face's list of connected faces
    m_p_faces[edgeIndex] = connectingFace;
}

/**
 * Takes in 2 vertices and returns the index of the edge on the face
 *
 * @param edgeVertices[] An array of 2 vertices on the face
 *
 * @return A signed int between 0 and 2 if valid edge vertices, -1 if invalid
 */
int16_t Mesh::Face::getEdgeIndex(shared_ptr<Mesh::Edge> p_edge) {
    // Find the index the given face belongs in according to edge order,
    // and then add the given face to this face's list of connected faces
    for (uint16_t i = 0; i < 3; ++i) {
        shared_ptr<const Mesh::Vertex> thisVert1 = m_p_vertices[i];
        shared_ptr<const Mesh::Vertex> thisVert2 = m_p_vertices[(i + 1) % 3];
        
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

bool Mesh::Face::intersectsPlane(const Plane & plane) const {
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(m_p_vertices[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(m_p_vertices[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(m_p_vertices[2]->vertex());
    
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

double Mesh::Face::area() const {
    return m_area;
}

const Vector3D & Mesh::Face::normal() const {
    return m_normal;
}

bool Mesh::Face::liesOnPlane(const Plane & plane) const {
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(m_p_vertices[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(m_p_vertices[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(m_p_vertices[2]->vertex());
    
    if ((vertexPos[0] == Plane::ON) && (vertexPos[1] == Plane::ON) && (vertexPos[2] == Plane::ON)) { //if any point lies exactly on plane, face intersects
        return true;
    } else {
        return false;
    }
}

pair<Vector3D, Vector3D> Mesh::Face::planeIntersection(const Plane & plane) const {
    if (!intersectsPlane(plane)) {
        writeLog(WARNING, "attempted to find intersection line of Mesh::Face with plane that does not intersect");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    } else if (liesOnPlane(plane)) {
        writeLog(WARNING, "attempted to find intersection line of Mesh::Face with plane that is parallel to face");
        return pair<Vector3D, Vector3D>(Vector3D(0, 0, 0), Vector3D(0, 0, 0));
    }
    
    Plane::PLANE_POSITION vertexPos[3];
    vertexPos[0] = plane.pointOnPlane(m_p_vertices[0]->vertex());
    vertexPos[1] = plane.pointOnPlane(m_p_vertices[1]->vertex());
    vertexPos[2] = plane.pointOnPlane(m_p_vertices[2]->vertex());
    
    //check if any vertices lie on plane
    if (vertexPos[0] == Plane::ON) { //first vertex lies on plane
        if (vertexPos[1] == Plane::ON) { //second vertex lies on plane
            //order of vertices depends in third vertex is above or below plane
            if (vertexPos[2] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(m_p_vertices[0]->vertex(), m_p_vertices[1]->vertex());
            } else { //vertexPos[2] cannot be on plane
                return pair<Vector3D, Vector3D>(m_p_vertices[1]->vertex(), m_p_vertices[0]->vertex());
            }
        } else if (vertexPos[2] == Plane::ABOVE) {
            if (vertexPos[1] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(m_p_vertices[2]->vertex(), m_p_vertices[0]->vertex());
            } else {
                return pair<Vector3D, Vector3D>(m_p_vertices[0]->vertex(), m_p_vertices[2]->vertex());
            }
        } else {
            return pair<Vector3D, Vector3D>(m_p_vertices[0]->vertex(), m_p_vertices[0]->vertex());
        }
    } else if (vertexPos[1] == Plane::ON) {
        //vertexPos[0] cannot be on plane
        if (vertexPos[2] == Plane::ON) {
            if (vertexPos[0] == Plane::ABOVE) {
                return pair<Vector3D, Vector3D>(m_p_vertices[1]->vertex(), m_p_vertices[2]->vertex());
            } else {
                return pair<Vector3D, Vector3D>(m_p_vertices[2]->vertex(), m_p_vertices[1]->vertex());
            }
        } else {
            return pair<Vector3D, Vector3D>(m_p_vertices[1]->vertex(), m_p_vertices[1]->vertex());
        }
    } else if (vertexPos[2] == Plane::ON) {
        return pair<Vector3D, Vector3D>(m_p_vertices[2]->vertex(), m_p_vertices[1]->vertex());
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
        writeLog(ERROR, "first intersection point of Mesh::Face edge and plane is not contained in edge");
    }
    if (t02 > p02.magnitude()) {
        writeLog(ERROR, "second intersection point of Mesh::Face edge and plane is not contained in edge");
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

string Mesh::Face::toString() const {
    ostringstream stream;
    stream << "[" << m_p_vertices[0]->toString() << ", " << m_p_vertices[1]->toString() << ", " << m_p_vertices[2]->toString() << "]";
    return stream.str();
}
