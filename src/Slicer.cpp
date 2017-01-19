//
//  Slicer.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/7/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "Slicer.hpp"

#include "Utility.hpp"

using namespace mapmqp;
using namespace std;

Slicer::Slicer(std::shared_ptr<const Mesh> p_mesh) :
m_p_mesh(p_mesh) { }

Slicer::Slice Slicer::slice(const Plane & plane) {
    //TODO is this a good way to do this?
    m_originalSlicingPlane = plane;
    m_currentSlicingPlane = m_originalSlicingPlane;
    vector<shared_ptr<const Mesh::Face>> p_faces;
    for (vector<shared_ptr<Mesh::Face>>::const_iterator it = m_p_mesh->p_faces().begin(); it != m_p_mesh->p_faces().end(); it++) {
        p_faces.push_back(*it);
    }
    pair<Slicer::Slice, vector<shared_ptr<const Mesh::Face>>> slicePair = slice(m_originalSlicingPlane, p_faces);

    m_searchSpace = slicePair.second;
    return slicePair.first;
}

Slicer::Slice Slicer::nextSlice() {
    // TODO: Using hard-coded 0.1mm layer resolution right now, this should be variable
    Plane newPlane = Plane(m_originalSlicingPlane.normal(), m_currentSlicingPlane.scalar() + 100);
    m_currentSlicingPlane = newPlane;
    m_searchSpace = expandSearchSpace(m_searchSpace, m_originalSlicingPlane, m_currentSlicingPlane);

    return slice(m_currentSlicingPlane, m_searchSpace).first;
}

/**
 * Takes a plane to slice a collection of mesh faces in. This plane
 * can be in any orientation and position relative to the origin
 *
 * @param plane A Plane object representing the slicing plane
 * @param p_facesSearchSpace A vector of pointers to mesh faces which will be sliced
 *
 * @return A pair containing the Slice object as its first element, and a vector of Mesh::Face
 *         pointers aligned with that slice as the second object
 */
pair<Slicer::Slice, vector<shared_ptr<const Mesh::Face>>> Slicer::slice(const Plane & plane, const vector<shared_ptr<const Mesh::Face>> & p_facesSearchSpace) const {
    // Create the return slice with the plane it's on
    Slice slice(plane, vector<shared_ptr<const Island>>());
    
    // Our vector of Mesh::Face objects located on the slice
    vector<shared_ptr<const Mesh::Face>> p_intersectingFaces;
    
    // List of islands pre-ordering
    vector<shared_ptr<Island>> p_islands;
    // List of hole polygons and corresponding Mesh::Faces
    vector<shared_ptr<Island>> p_holes;

    // Stores all the discovered polygons before they're split off into holes and islands
    vector<Polygon> polygons;
    
    // Stores the vertices that have already been evaluated by mapping its three vertices to a reference to the face
    Identifier<Mesh::Face>::Roster checkedFacesRoster;
    // Iterate through all faces in the search space
    for (vector<shared_ptr<const Mesh::Face>>::const_iterator it = p_facesSearchSpace.begin(); it != p_facesSearchSpace.end(); it++) {
        // Grab the actual Mesh::Face pointer from the iterator
        shared_ptr<const Mesh::Face> p_face = *it;
        
        // Get some information about the face in relation to the slice
        bool alreadyMapped = checkedFacesRoster.contains(*p_face);
        bool intersectsPlane = p_face->intersectsPlane(plane);
        bool liesOnPlane = p_face->liesOnPlane(plane);
        
        // Check we've never seen this face, it intersects the plane, and it doesn't lie on the plane
        if (!alreadyMapped && intersectsPlane && !liesOnPlane) {
            // Cycle around faces until circle is complete
            vector<Vector3D> polygonPoints;
            vector<shared_ptr<const Mesh::Face>> p_polygonMeshFaces;
            
            shared_ptr<const Mesh::Face> p_startFace = p_face;
            shared_ptr<const Mesh::Face> p_currentFace = p_startFace;
            Vector3D prevIntersectionPoint;
            
            int processedFaceCount = 0;
            do {
                writeLog(INFO, "Processing face: %s", p_currentFace->toString().c_str());
                processedFaceCount++;
                //add ptr to Mesh::Face to list of checked faces
                checkedFacesRoster.add(*p_currentFace);
                
                //add ptr to Mesh::Face to hashtable of intersection faces
                p_intersectingFaces.push_back(p_currentFace);
                
                pair<Vector3D, Vector3D> intersectionLine = p_currentFace->planeIntersection(plane);

                // This ensures we always move "forward" in slicing by always making the second intersection point
                // be the one that touches new faces
                if (processedFaceCount > 0) {
                    if (intersectionLine.first != prevIntersectionPoint && intersectionLine.second == prevIntersectionPoint) {
                        intersectionLine.second = intersectionLine.first;
                        intersectionLine.first = prevIntersectionPoint;
                    }
                }
                prevIntersectionPoint = intersectionLine.second;

                writeLog(INFO, "Intersection line (%s, %s)", intersectionLine.first.toString().c_str(), intersectionLine.second.toString().c_str());

                // add first point of face intersection to list of polygon points, only if the two points don't match
                // if the two points match, it means the face intersects with the plane exactly on a vertex, therefore
                // the next face processed that has more than one point intersecting with the plane will also add that 
                // point in
                if (intersectionLine.first != intersectionLine.second) {
                    polygonPoints.push_back(intersectionLine.first);
                }
                p_polygonMeshFaces.push_back(p_currentFace);

                Vector3D edge0 = Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(0)->vertex(), p_currentFace->p_vertex(1)->vertex() - p_currentFace->p_vertex(0)->vertex());
                Vector3D edge1 = Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(1)->vertex(), p_currentFace->p_vertex(2)->vertex() - p_currentFace->p_vertex(1)->vertex());
                Vector3D edge2 = Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(2)->vertex(), p_currentFace->p_vertex(0)->vertex() - p_currentFace->p_vertex(2)->vertex());
                Vector3D firstEdge0 = Vector3D::crossProduct(intersectionLine.first - p_currentFace->p_vertex(0)->vertex(), p_currentFace->p_vertex(1)->vertex() - p_currentFace->p_vertex(0)->vertex());
                Vector3D firstEdge1 = Vector3D::crossProduct(intersectionLine.first - p_currentFace->p_vertex(1)->vertex(), p_currentFace->p_vertex(2)->vertex() - p_currentFace->p_vertex(1)->vertex());
                Vector3D firstEdge2 = Vector3D::crossProduct(intersectionLine.first - p_currentFace->p_vertex(2)->vertex(), p_currentFace->p_vertex(0)->vertex() - p_currentFace->p_vertex(2)->vertex());
                double secondDotProds[3] = { Vector3D::dotProduct(edge0, p_currentFace->normal()), Vector3D::dotProduct(edge1, p_currentFace->normal()), Vector3D::dotProduct(edge2, p_currentFace->normal()) };
                double firstDotProds[3] = { Vector3D::dotProduct(firstEdge0, p_currentFace->normal()), Vector3D::dotProduct(firstEdge1, p_currentFace->normal()), Vector3D::dotProduct(firstEdge2, p_currentFace->normal()) };

                writeLog(INFO, "Second intersection point:");
                writeLog(INFO, "\tEdge #1: %s, dot prod: %f, magnitude: %f", edge0.toString().c_str(), secondDotProds[0], edge0.magnitude());
                writeLog(INFO, "\tEdge #2: %s, dot prod: %f, magnitude: %f", edge1.toString().c_str(), secondDotProds[1], edge1.magnitude());
                writeLog(INFO, "\tEdge #3: %s, dot prod: %f, magnitude: %f", edge2.toString().c_str(), secondDotProds[2], edge2.magnitude());
                
                writeLog(INFO, "First intersection point:");
                writeLog(INFO, "\tEdge #1: %s, dot prod: %f, magnitude: %f", firstEdge0.toString().c_str(), firstDotProds[0], firstEdge0.magnitude());
                writeLog(INFO, "\tEdge #2: %s, dot prod: %f, magnitude: %f", firstEdge1.toString().c_str(), firstDotProds[1], firstEdge1.magnitude());
                writeLog(INFO, "\tEdge #3: %s, dot prod: %f, magnitude: %f", firstEdge2.toString().c_str(), firstDotProds[2], firstEdge2.magnitude());

                //determine which edge of face is next depending on intersection with the plane and already visited status
                bool alreadyVisited[3];
                bool intersectsPlane[3];
                bool liesOnPlane[3];
                for (int i = 0; i < 3; ++i) {
                    shared_ptr<const Mesh::Face> neighbor = p_currentFace->p_connectedFace(i);

                    alreadyVisited[i] = checkedFacesRoster.contains(*(p_currentFace->p_connectedFace(i)));
                    intersectsPlane[i] = p_currentFace->p_connectedFace(i)->intersectsPlane(plane);//Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(i)->vertex(), p_currentFace->p_vertex(i + 1 == 3 ? 0 : i + 1)->vertex() - p_currentFace->p_vertex(i)->vertex()).magnitude() == 0;
                    liesOnPlane[i] = p_currentFace->p_connectedFace(i)->liesOnPlane(plane);

                    writeLog(INFO, "\tChecking neighbor: %s, %d, %d, %d", p_currentFace->p_connectedFace(i)->toString().c_str(), alreadyVisited[i], intersectsPlane[i], liesOnPlane[i]);
                }

                for (int i = 0; i < 3; ++i) {
                    if (!alreadyVisited[i] && intersectsPlane[i] && !liesOnPlane[i] && doubleEquals(secondDotProds[i], 0.0)) {
                        p_currentFace = p_currentFace->p_connectedFace(i);
                        break;
                    }

                    // There must be, minimum, three faces for a 3-D shape to have a closed loop, therefore we can only be back at the start if we've processed at least 2 faces
                    if (processedFaceCount > 2 && p_currentFace->p_connectedFace(i) == p_startFace) p_currentFace = p_startFace;
                }
                
                if (!p_currentFace) {
                    writeLog(ERROR, "connected face to face being sliced is null");
                    break;
                }
            } while (p_currentFace != p_startFace);
            
            writeLog(INFO, "slicing face: %s", p_startFace->toString().c_str());
            writeLog(INFO, "polygonPoints size: %d", polygonPoints.size());
            for (int i = 0; i < polygonPoints.size(); ++i) {
                writeLog(INFO, "%d) %s", i + 1, polygonPoints[i].toString().c_str());
            }
            Polygon poly(polygonPoints);

            // Assume all polygons are islands, not holes, initially
            // if (poly.area() < 0) {
            //     poly.togglePolygonType();
            // }

            // polygons.push_back(poly);

            writeLog(INFO, "poly area: %f", poly.area());
            if (poly.area() < 0) { //polygon is a hole
                p_holes.push_back(shared_ptr<Island>(new Island(poly, p_polygonMeshFaces, true)));
            } else {
                p_islands.push_back(shared_ptr<Island>(new Island(poly, p_polygonMeshFaces)));
            }
        }
    }
    
    //if holes exist, we need to determine which islands they belong to and if any islands are inside holes
    if (p_holes.size() > 0) {
        //check if each island belongs in a hole
        for (vector<shared_ptr<Island>>::iterator islandIt = p_islands.begin(); islandIt != p_islands.end(); islandIt++) {
            shared_ptr<Island> p_island = *islandIt;
            Vector3D firstPoint = p_island->polygon().points()[0];
            
            shared_ptr<Island> p_parentHole = nullptr; //which hole (if any) island is inside of
            for (vector<shared_ptr<Island>>::iterator holeIt = p_holes.begin(); holeIt != p_holes.end(); holeIt++) {
                shared_ptr<Island> p_hole = *holeIt;
                
                //if polygon is in hole
                if (p_hole->polygon().pointInPolygon(firstPoint)) {
                    if (p_parentHole) {
                        //smaller parent is more immediate parent
                        if (p_hole->polygon().area() < p_parentHole->polygon().area()) {
                            p_parentHole = p_hole;
                        }
                    } else {
                        p_parentHole = p_hole;
                    }
                }
            }
            
            if (!p_parentHole) { //no parent hole means island is top level island
                slice.islands().push_back(*islandIt);
            }
        }
        
        //check which islands each hole belongs to
        for (vector<shared_ptr<Island>>::iterator holeIt = p_holes.begin(); holeIt != p_holes.end(); holeIt++) {
            shared_ptr<Island> p_hole = *holeIt;
            Vector3D firstPoint = p_hole->polygon().points()[0];
            
            shared_ptr<Island> p_parentIsland = nullptr; //which island hole is inside of
            for (vector<shared_ptr<Island>>::iterator islandIt = p_islands.begin(); islandIt != p_islands.end(); islandIt++) {
                shared_ptr<Island> p_island = *islandIt;
                
                //if hole is in island
                if (p_island->polygon().pointInPolygon(firstPoint)) {
                    if (p_parentIsland) {
                        //smaller parent is more immediate parent
                        if (p_parentIsland->polygon().area() < p_parentIsland->polygon().area()) {
                            p_parentIsland = p_hole;
                        }
                    } else {
                        p_parentIsland = p_hole;
                    }
                }
            }
            
            if (!p_parentIsland) { //every hole should have a parent island
                writeLog(ERROR, "slice generated hole without a parent island");
            }
        }
        
    } else { // if no holes exist, all islands are disjoint
        for (vector<shared_ptr<Island>>::iterator it = p_islands.begin(); it != p_islands.end(); it++) {
            slice.islands().push_back(*it);
        }
    }
    return pair<Slice, vector<shared_ptr<const Mesh::Face>>>(slice, p_intersectingFaces);
}

/**
 * Takes the shape of the slice and turns it into a 2D polygon
 *
 * @return A vector of polygons containing all the polygons in the slice
 */
std::vector<Polygon> Slicer::Slice::toPoly() {
    std::vector<Polygon> allPolys;

    for (std::shared_ptr<const Island> island : m_p_islands) {
        island->toPoly(allPolys);
    }

    return allPolys;
}

/**
 * Getter returns the vector of islands in the slice
 *
 * @return Reference to the vector containing all of the slice's islands
 */
vector<shared_ptr<const Island>> & Slicer::Slice::islands() {
    return m_p_islands;
}

/**
 * Getter returns the slice's plane
 *
 * @return A Plane object
 */
Plane Slicer::Slice::plane() const {
    return m_plane;
}

/**
 * Retrieves all faces on all islands of the slice
 *
 * @return A vector of mesh face pointers
 */
vector<shared_ptr<const Mesh::Face>> Slicer::Slice::faces() const {
    vector<shared_ptr<const Mesh::Face>> allFaces;

    for (shared_ptr<const Island> island : m_p_islands) {
        vector<shared_ptr<const Mesh::Face>> islandFaces = island->allFaces();

        for (shared_ptr<const Mesh::Face> islandFace : islandFaces) {
            allFaces.push_back(islandFace);
        }
    }

    return allFaces;
}

/**
 * Takes in all faces that intersect a plane and returns all faces that intersect the next plane
 *
 * @param p_facesSearchSpace all faces that intersect the previous plane
 * @param originalPlane the plane of the previous slice
 * @param nextPlane the plane of the next slice
 *
 * @return all faces that intersect the next slice plane
 */
std::vector<std::shared_ptr<const Mesh::Face>> Slicer::expandSearchSpace(std::vector<std::shared_ptr<const Mesh::Face>> & p_facesSearchSpace, const Plane & originalPlane, const Plane & nextPlane) const {
    if (originalPlane.pointOnPlane(nextPlane.origin()) != Plane::ABOVE) {
        writeLog(ERROR, "attempting to expand search space to slice not above previous slice");
        return p_facesSearchSpace;
    }
    
    std::vector<std::shared_ptr<const Mesh::Face>> p_facesSearchSpaceExpanded;
    
    Identifier<Mesh::Face>::Roster faceRoster; //used to check if a face has been searched
    for (vector<shared_ptr<const Mesh::Face>>::iterator it = p_facesSearchSpace.begin(); it != p_facesSearchSpace.end(); it++) {
        if (!faceRoster.contains(**it)) {
            queue<shared_ptr<const Mesh::Face>> queue;
            queue.push(*it);
            while (queue.size() > 0) {
                //take first element in queue
                shared_ptr<const Mesh::Face> p_face = queue.front();
                queue.pop();
                
                faceRoster.add(*p_face); //mark face as searched
                
                bool intersectsNextPlane = false; //whether of not the face intersects the next plane
                bool entirelyAboveNextPlane = true; //whether or not face lies entirely on/above next plane
                
                Plane::PLANE_POSITION positions[3]; //calculate plane position of each point
                for (unsigned int i = 0; i < 3; i++) {
                    positions[i] = originalPlane.pointOnPlane(p_face->p_vertex(i)->vertex());
                    
                    Plane::PLANE_POSITION nextPlanePos = nextPlane.pointOnPlane(p_face->p_vertex(i)->vertex());
                    intersectsNextPlane |= (nextPlanePos != Plane::BELOW);
                    entirelyAboveNextPlane &= (nextPlanePos != Plane::BELOW);
                }
                
                if (intersectsNextPlane) {
                    p_facesSearchSpaceExpanded.push_back(p_face);
                }
                
                if (!entirelyAboveNextPlane) {
                    for (unsigned int i = 0; i < 3; i++) { //check each edge of face
                        Vector3D pFirst = positions[i];
                        Vector3D pSecond = positions[i + 1];
                        if ((originalPlane.pointOnPlane(pFirst) == Plane::ABOVE) || (originalPlane.pointOnPlane(pSecond) == Plane::ABOVE)) { //if edge contains point above plane, add neighboring face of edge to queue
                            shared_ptr<const Mesh::Face> p_neighbor = p_face->p_connectedFace(i);
                            if (!faceRoster.contains(*p_neighbor)) { //only add face if it has not been looked at yet
                                queue.push(p_neighbor);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return p_facesSearchSpaceExpanded;
}