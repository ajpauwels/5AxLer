#include "ProcessSTL.hpp"
#include "Utility.hpp"
#include <fstream>
#include <math.h>

using namespace mapmqp;
using namespace std;

ProcessSTL::ProcessSTL(string stlFilePath) : m_p_mesh(new Mesh()) {
    m_stlFilePath = stlFilePath;
}

/**
 * Runs the STL parser on the provided STL file.
 *
 * @return True if success, false otherwise
 */
shared_ptr<Mesh> ProcessSTL::run() {
    constructMeshFromSTL();
    
    return m_p_mesh;
}

/**
 * Takes a pointer to a file handler and opens the STL
 * specified in stlFile_
 *
 * @param file Point to the ifstream object
 *
 * @return True if success, false otherwise
 */
bool ProcessSTL::getFileHandler(ifstream & file) {
    file.open(this->m_stlFilePath.c_str(), ios::in | ios::binary);
    
    return file.is_open();
}

/**
 * Takes a MeshVertex pointer and attempts to add it to the
 * vector of vertices. If it has already been added, this
 * function returns a shared pointer to the already existing point.
 * Otherwise, it returns the same pointer as was given.
 *
 * @param A MeshVertex shared pointer to add to the unordered map
 *
 * @return A shared pointer to the MeshVertex in the unordered map
 */
shared_ptr<MeshVertex> ProcessSTL::addMeshVertex(shared_ptr<MeshVertex> p_vertex) {
    shared_ptr<MeshVertex> finalVertex;
    // Add the vertex to the vertices list unless it's already there
    pair<unordered_map<Vector3D, shared_ptr<MeshVertex>, Vector3DHash>::iterator, bool> emplacePair = m_mapped_p_vertices.emplace(p_vertex->vertex(), p_vertex); //place MeshVertex ptr into hashtable
    finalVertex = emplacePair.first->second; //set MeshVertex ptr to returned value from hashtable in case it has changed
    if (emplacePair.second) { //if MeshVertex did not exist in hashtable, add to list of vertices
        m_p_mesh->addVertex(finalVertex);
    }
    
    return finalVertex;
}

/**
 * Takes a MeshFace pointer and adds it to the vector
 * of faces, connecting any faces it may share an edge with.
 *
 * @param face The MeshFace to add
 */
void ProcessSTL::addMeshFace(shared_ptr<MeshFace> p_face) {
    // Gather vertices and create edges
    shared_ptr<const MeshVertex> p_vert1 = p_face->p_vertex(0);
    shared_ptr<const MeshVertex> p_vert2 = p_face->p_vertex(1);
    shared_ptr<const MeshVertex> p_vert3 = p_face->p_vertex(2);
    shared_ptr<MeshEdge> p_edge1(new MeshEdge(p_vert1, p_vert2));
    shared_ptr<MeshEdge> p_edge2(new MeshEdge(p_vert2, p_vert3));
    shared_ptr<MeshEdge> p_edge3(new MeshEdge(p_vert3, p_vert1));
    shared_ptr<MeshEdge> edges[3] = {p_edge1, p_edge2, p_edge3};
    
    Vector3D tv1(38100, 0, 25400);
    Vector3D tv2(38100, 76200, 25400);
    shared_ptr<MeshVertex> tmv1(new MeshVertex(tv1));
    shared_ptr<MeshVertex> tmv2(new MeshVertex(tv2));
    shared_ptr<MeshEdge> tme1(new MeshEdge(tmv1, tmv2));
    
    // Check each edge against the map of edges
    for (unsigned int i = 0; i < 3; ++i) {
        // Hash the MeshEdge to the map of edges
        pair<unordered_map<shared_ptr<MeshEdge>, shared_ptr<MeshFace>, MeshEdgePtrHash, MeshEdgePtrEquality>::iterator, bool> emplacePair = m_mapped_p_edges.emplace(edges[i], p_face);
        
        printf("EDGE: %s, %s\n", edges[i]->p_vertex(0)->vertex().toString().c_str(), edges[i]->p_vertex(1)->vertex().toString().c_str());
        
        // Get the resulting MeshEdge and MeshFace from the hash
        shared_ptr<MeshEdge> hashedEdge = emplacePair.first->first;
        shared_ptr<MeshFace> hashedFace = emplacePair.first->second;
        
        // Get whether the MeshFace was added
        bool faceWasAdded = emplacePair.second;
        
        MeshEdgePtrEquality mepe;
        
        if (*edges[i] == *tme1) {
            printf("MATCHED, faceWasAdded = %d, %d\n", faceWasAdded, mepe(edges[i], tme1));
        }
        
        // If there was already a face at the edge, do some error checking and connect them
        if (!faceWasAdded) {
            // If the exact same face has already been added to the mesh, it's logical that
            // it would have the exact same 3 edges. So we only check the first edge to see
            // if the already added face is the same face
            if (i == 0) {
                if (hashedFace == p_face) {
                    writeLog(WARNING, "attempted to add a face to the mesh that has already been added, skipping");
                    return;
                }
            }
            
            // Connect the two faces
            int16_t hashedFaceEdgeIndex = hashedFace->getEdgeIndex(hashedEdge);
            if (hashedFaceEdgeIndex == -1) {
                writeLog(ERROR, "edge hashed to a face but face did not have that edge, removed face from the hash for that edge");
                return;
            }
            
            hashedFace->connect(p_face, hashedFace->getEdgeIndex(hashedEdge));
            
            p_face->connect(hashedFace, i);
            
            // Remove the edge from the hashmap since it's only used for the two connected triangles
            m_mapped_p_edges.erase(emplacePair.first);
        }
    }
    
    // Finally, add the face to the mesh list of faces
    m_p_mesh->addFace(p_face);
}

/**
 * When called, uses the STL file path provided from the
 * constructor to generate a Mesh object from the file.
 * The data structure is as follows:
 * 		- The Mesh object contains a vector of MeshVertex and MeshFace
 *		- Each MeshVertex stores its x/y/z vector and pointers to all the faces connected to it
 *		- Each MeshFace stores its three MeshVertex object and pointers all of the faces connected to it
 *		- The vector of Vertex objects of the MeshFace are arranged in counter-clockwise order
 *		- The connecting face 0 for each MeshFace is the one attached to the edge between vertex 0 and 1, etc.
 * This way, we have a graph of both vertices and faces we can use to navigate the object.
 */
void ProcessSTL::constructMeshFromSTL() {
    ifstream file;									// Our file handler
    char *header = new char[80];					// The 80-char file header
    unsigned int size;								// The number of triangles in the file
    
    writeLog(INFO, "parsing STL file %s...", m_stlFilePath.c_str());
    if (getFileHandler(file)) {                        // Check that we opened successfully
        file.read(header, 80);							// Get the header
        file.read((char*)&size, 4);						// Get the number of triangles
        
        writeLog(INFO, "number of triangles: %d", size);

        double lowestZVal = INFINITY;
        
        for (unsigned int i = 0; i < size; ++i) {		// Loop through all triangles
            Vector3D norm, vertices[3];                 // Stores the three triangle points + normal vector
            float points[12] = { };						// 4 vectors * 3 points = 12 points
            short abc;									// Stores the attribute byte count
            
            for (unsigned int j = 0; j < 12; ++j) {		// Get all points from file
                file.read((char*)(points + j), 4);
                points[j] = floor((points[j] * 1000) + 0.5);
            }
            file.read((char*)&abc, 2);
            
            norm = Vector3D(points[0], points[1], points[2]);           // Create normal vector
            vertices[0] = Vector3D(points[3], points[4], points[5]);	// Get first point of triangle
            vertices[1] = Vector3D(points[6], points[7], points[8]);	// Get second point of triangle
            vertices[2] = Vector3D(points[9], points[10], points[11]);	// Get third point of triangle

            shared_ptr<MeshVertex> p_meshVertices[3]; //Three MeshVertex pointers
            
            // Process the three vertices of the triangle
            for (unsigned int i = 0; i < 3; i++) {
                shared_ptr<MeshVertex> p_meshVertex(new MeshVertex(vertices[i]));
                p_meshVertices[i] = addMeshVertex(p_meshVertex);
                
                // If the lowestVertex was never set or the current vertex is lower than the lowestVertex, replace
                // lowestVertex with the current vertex
                if (p_meshVertices[i]->vertex().z() == lowestZVal) {
                    m_p_lowestVertices.push_back(p_meshVertices[i]);
                }
                // If a lower vertex was encountered, reset the lowest vertices list
                else if (p_meshVertices[i]->vertex().z() < lowestZVal) {
                    lowestZVal = p_meshVertices[i]->vertex().z();
                    m_p_lowestVertices.clear();
                    m_p_lowestVertices.push_back(p_meshVertices[i]);
                }
            }
            
            // Create new MeshFace from vertices
            shared_ptr<MeshFace> p_meshFace(new MeshFace(p_meshVertices[0], p_meshVertices[1], p_meshVertices[2]));
            
            // Add face to the mesh and connect it to its surrounding faces
            addMeshFace(p_meshFace);
            
            // Add the new MeshFace to the list of connected faces of all its vertices to connect vertices
            for (unsigned int i = 0; i < 3; i++) {
                p_meshVertices[i]->addConnectedFace(p_meshFace);
            }
        }
        file.close();	// Close the file
    } else {
        writeLog(ERROR, "unable to open file %s [errno: %d]", m_stlFilePath.c_str(), strerror(errno));
    }
}
