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

Slice Slicer::slice(const Plane & plane) const {
    //TODO is this a good way to do this?
    vector<shared_ptr<const MeshFace>> p_faces;
    for (vector<shared_ptr<MeshFace>>::const_iterator it = m_p_mesh->p_faces().begin(); it != m_p_mesh->p_faces().end(); it++) {
        p_faces.push_back(*it);
    }
    return slice(plane, p_faces).first;
}

pair<Slice, vector<shared_ptr<const MeshFace>>> Slicer::slice(const Plane & plane, const vector<shared_ptr<const MeshFace>> & p_facesSearchSpace) const {
    Slice slice(plane);
    vector<shared_ptr<const MeshFace>> p_intersectingFaces;
    
    //used for hashing ptrs to MeshFaces
    struct MeshFaceHash {
        size_t operator()(const MeshFace & meshFace) const {
            return hash<shared_ptr<const MeshVertex>>()(meshFace.p_vertex(0)) ^ hash<shared_ptr<const MeshVertex>>()(meshFace.p_vertex(1)) ^ hash<shared_ptr<const MeshVertex>>()(meshFace.p_vertex(2));
        }
    };
    
    unordered_map<MeshFace, shared_ptr<const MeshFace>, MeshFaceHash> mapped_p_checkedFaces;
    
    //list of islands pre-ordering
    vector<shared_ptr<Island>> p_islands;
    //list of hole polygons and corresponding MeshFaces
    vector<shared_ptr<Island::Hole>> p_holes;
    
    for (vector<shared_ptr<const MeshFace>>::const_iterator it = p_facesSearchSpace.begin(); it != p_facesSearchSpace.end(); it++) {
        shared_ptr<const MeshFace> p_face = *it;
        
        writeLog(INFO, "slicing MeshFace: %s", p_face->toString().c_str());
        
        if ((mapped_p_checkedFaces.size() > 0) && (mapped_p_checkedFaces.find(*p_face) != mapped_p_checkedFaces.end())) { //face has already been evaluated
            continue;
        } else if (!(p_face->intersectsPlane(plane))) { //face does not intersect plane
            continue;
        } else if (p_face->liesOnPlane(plane)) {
            continue;
        }
        
        //cycle around faces until circle is complete
        vector<Vector3D> polygonPoints;
        vector<shared_ptr<const MeshFace>> p_polygonMeshFaces;
        
        //TODO determine somehow if polygon is hole inside of other polygon
        
        shared_ptr<const MeshFace> p_startFace = p_face;
        shared_ptr<const MeshFace> p_currentFace = p_startFace;
        
        do {
            //add ptr to MeshFace to list of checked faces
            mapped_p_checkedFaces.emplace(tuple<shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>>(p_currentFace->p_vertex(0), p_currentFace->p_vertex(1), p_currentFace->p_vertex(2)), p_currentFace);
            
            //add ptr to MeshFace to hashtable of intersection faces
            p_intersectingFaces.push_back(p_currentFace);
            
            pair<Vector3D, Vector3D> intersectionLine = p_currentFace->planeIntersection(plane);
            
            //add first point of face intersection to list of polygon points
            polygonPoints.push_back(intersectionLine.first);
            p_polygonMeshFaces.push_back(p_currentFace);
            
            //determine which edge of face is next
            if (Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(0)->vertex(), p_currentFace->p_vertex(1)->vertex() - p_currentFace->p_vertex(0)->vertex()).magnitude() == 0) {
                p_currentFace = p_currentFace->p_connectedFace(0);
            } else if (Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(1)->vertex(), p_currentFace->p_vertex(2)->vertex() - p_currentFace->p_vertex(1)->vertex()).magnitude() == 0) {
                p_currentFace = p_currentFace->p_connectedFace(1);
            } else if (Vector3D::crossProduct(intersectionLine.second - p_currentFace->p_vertex(2)->vertex(), p_currentFace->p_vertex(0)->vertex() - p_currentFace->p_vertex(2)->vertex()).magnitude() == 0) {
                p_currentFace = p_currentFace->p_connectedFace(2);
            }
            
            if (!p_currentFace) {
                writeLog(ERROR, "connected face to face being sliced is nullptr");
                break;
            }
        } while (p_currentFace != p_startFace);
        
        Polygon poly(polygonPoints);
        if (poly.area() < 0) { //polygon is a hole
            p_holes.push_back(shared_ptr<Island::Hole>(new Island::Hole(poly, p_polygonMeshFaces)));
        } else {
            p_islands.push_back(shared_ptr<Island>(new Island(poly, p_polygonMeshFaces)));
        }
    }
    
    //all islands and holes found
    
    //if holes exist, we need to determine which islands they belong to and if any islands are inside holes
    if (p_holes.size() > 0) {
        //check if each island belongs in a hole
        for (vector<shared_ptr<Island>>::iterator islandIt = p_islands.begin(); islandIt != p_islands.end(); islandIt++) {
            shared_ptr<Island> p_island = *islandIt;
            Vector3D firstPoint = p_island->polygon().points()[0];
            
            shared_ptr<Island::Hole> p_parentHole = nullptr; //which hole (if any) island is inside of
            for (vector<shared_ptr<Island::Hole>>::iterator holeIt = p_holes.begin(); holeIt != p_holes.end(); holeIt++) {
                shared_ptr<Island::Hole> p_hole = *holeIt;
                
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
                slice.addIsland(*islandIt);
            }
        }
        
        //check which islands each hole belongs to
        for (vector<shared_ptr<Island::Hole>>::iterator holeIt = p_holes.begin(); holeIt != p_holes.end(); holeIt++) {
            shared_ptr<Island::Hole> p_hole = *holeIt;
            Vector3D firstPoint = p_hole->polygon().points()[0];
            
            shared_ptr<Island::Hole> p_parentIsland = nullptr; //which island hole is inside of
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
            slice.addIsland(*it);
        }
    }
    
    return pair<Slice, vector<shared_ptr<const MeshFace>>>(slice, p_intersectingFaces);
}
