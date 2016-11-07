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
p_mesh_(p_mesh) { }

Slice Slicer::slice(const Plane & plane) const {
    //TODO is this a good way to do this?
    vector<shared_ptr<const MeshFace>> p_faces;
    for (vector<shared_ptr<MeshFace>>::const_iterator it = p_mesh_->p_faces().begin(); it != p_mesh_->p_faces().end(); it++) {
        p_faces.push_back(*it);
    }
    return slice(plane, p_faces).first;
}

pair<Slice, vector<shared_ptr<const MeshFace>>> Slicer::slice(const Plane & plane, const vector<shared_ptr<const MeshFace>> & p_facesSearchSpace) const {
    Slice ret(plane);
    vector<shared_ptr<const MeshFace>> p_intersectingFaces;
    
    //used for hashing ptrs to MeshFaces
    struct TripleP_MeshVertexHash {
        size_t operator()(const tuple<shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>> & p_vertices) const {
            return hash<shared_ptr<const MeshVertex>>()(get<0>(p_vertices)) ^ hash<shared_ptr<const MeshVertex>>()(get<1>(p_vertices)) ^ hash<shared_ptr<const MeshVertex>>()(get<2>(p_vertices));
        }
    };
    
    unordered_map<tuple<shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>>, shared_ptr<const MeshFace>, TripleP_MeshVertexHash> mapped_p_checkedFaces;
    
    //list of islands pre-ordering
    vector<shared_ptr<Island>> p_islands;
    //list of hole polygons and corresponding MeshFaces
    vector<pair<Polygon, vector<shared_ptr<const MeshFace>>>> holes;
    
    for (vector<shared_ptr<const MeshFace>>::const_iterator it = p_facesSearchSpace.begin(); it != p_facesSearchSpace.end(); it++) {
        shared_ptr<const MeshFace> p_face = *it;
        
        printf("slicing MeshFace: %s\n", p_face->toString().c_str());
        
        if ((mapped_p_checkedFaces.size() > 0) && (mapped_p_checkedFaces.find(tuple<shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>, shared_ptr<const MeshVertex>>(p_face->p_vertex(0), p_face->p_vertex(1), p_face->p_vertex(2))) != mapped_p_checkedFaces.end())) { //face has already been evaluated
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
            holes.push_back(pair<Polygon, vector<shared_ptr<const MeshFace>>>(poly, p_polygonMeshFaces));
        } else {
            p_islands.push_back(shared_ptr<Island>(new Island(poly, p_polygonMeshFaces)));
        }
    }
    
    //all islands and holes found
    
    //place islands in holes
    
    //TODO this is just temporary to return something
    for (vector<shared_ptr<Island>>::iterator it = p_islands.begin(); it != p_islands.end(); it++) {
        ret.addIsland(**it);
    }
    
//    vector<shared_ptr<Island>> finalizedIslands;
//    
//    vector<shared_ptr<Island>> searchSpace;
//    //TODO can this just be set to = p_islands?
//    for (vector<shared_ptr<Island>>::iterator it = p_islands.begin(); it != p_islands.end(); it++) {
//        searchSpace.push_back(*it);
//    }
//    
//    int depth = 0;
//    while (holes.size() > 0) {
//        if (searchSpace.size() == 0) {
//            printf("SEARCH SPACE IS 0\n");
//        }
//        
//        vector<vector<shared_ptr<Island>>::iterator> p_topLevelPolysIterators;
//        vector<vector<pair<Polygon, vector<shared_ptr<const MeshFace>>>>::iterator> topLevelHolePolyIterators; //holes that are only a subset of one island at level being evaluated - to be removed from holePolygons
//        
//        for (vector<pair<Polygon, vector<shared_ptr<const MeshFace>>>>::iterator holeIt = holes.begin(); holeIt != holes.end(); holeIt++) {
//            vector<vector<shared_ptr<Island>>::iterator> enclosingIslandIterators; //islands that contain hole
//            
//            for (vector<shared_ptr<Island>>::iterator islandIt = searchSpace.begin(); islandIt != searchSpace.end(); islandIt++) {
//                if ((*islandIt)->mainPolygon().pointInPolygon(holeIt->first.points()[0])) { //polygon contains hole
//                    enclosingIslandIterators.push_back(islandIt);
//                }
//            }
//            
//            if (enclosingIslandIterators.size() == 0) {
//                writeLog(ERROR, "hole exists that is not enclosed by an Island");
//            } else if (enclosingIslandIterators.size() == 1) {
//                shared_ptr<Island> p_island = *(enclosingIslandIterators[0]);
//                
//                //add hole to island
//                p_island->addHole(holeIt->first, holeIt->second);
//                
//                //if island does not already have hole, add it to list of top level islands
//                bool inList = false;
//                for (vector<vector<shared_ptr<Island>>::iterator>::iterator it = p_topLevelPolysIterators.begin(); it != p_topLevelPolysIterators.end(); it++) {
//                    inList |= (*it == enclosingIslandIterators[0]);
//                }
//                if (!inList) {
//                    p_topLevelPolysIterators.push_back(enclosingIslandIterators[0]);
//                    if (depth == 0) {
//                        finalizedIslands.push_back(p_island);
//                    }
//                }
//                
//                //add hole to list of top level island holes
//                topLevelHolePolyIterators.push_back(holeIt);
//            }
//        }
//        
//        //remove all islands with holes added from list of islands
//        for (vector<vector<shared_ptr<Island>>::iterator>::iterator it = p_topLevelPolysIterators.begin(); it != p_topLevelPolysIterators.end(); it++) {
//            searchSpace.erase(*it);
//        }
//        //remove all top level holes from hole polygons list
//        for (vector<vector<pair<Polygon, vector<shared_ptr<const MeshFace>>>>::iterator>::iterator it = topLevelHolePolyIterators.begin(); it != topLevelHolePolyIterators.end(); it++) {
//            holes.erase(*it);
//        }
//        
//        if (depth == 0) {
//            //cycle through remaining polygons and check to see if they are inside
//            //if poly is not in any holes then it is a top level island
//            vector<shared_ptr<Island>> newSearchSpace;
//            for (vector<shared_ptr<Island>>::iterator it = searchSpace.begin(); it != searchSpace.end(); it++) {
//                shared_ptr<Island> p_poly = *it;
//                
//                bool inHole = false;
//                for (vector<vector<shared_ptr<Island>>::iterator>::iterator subIt = p_topLevelPolysIterators.begin(); subIt != p_topLevelPolysIterators.end(); subIt++) { //check every polygon with a hole if it contains polygon in it's hole
//                    shared_ptr<Island> p_topLevelPoly = **subIt;
//                    
//                    //if polygon lies in hole of top level polygon, keep in search space
//                    for (vector<Polygon>::const_iterator holeIt = p_topLevelPoly->holes().begin(); holeIt != p_topLevelPoly->holes().end(); holeIt++) {
//                        if (holeIt->pointInPolygon(p_poly->mainPolygon().points()[0])) {
//                            inHole = true;
//                            //TODO place polygon in holeo
//                            break;
//                        }
//                    }
//                    if (inHole) {
//                        break;
//                    }
//                }
//                
//                if (!inHole) {
//                    finalizedIslands.push_back(p_poly);
//                } else {
//                    newSearchSpace.push_back(p_poly);
//                }
//            }
//            searchSpace = newSearchSpace;
//        } else {
//            vector<shared_ptr<Island>> newSearchSpace;
//            
//            vector<shared_ptr<Island>> p_islandsToCheck;
//            for (vector<shared_ptr<Island>>::iterator it = finalizedIslands.begin(); it != finalizedIslands.end(); it++) {
//                vector<shared_ptr<Island>> p_subIslands = (*it)->getAllP_SubIslands(depth);
//                for (vector<shared_ptr<Island>>::iterator subIt = p_subIslands.begin(); subIt != p_subIslands.end(); subIt++) {
//                    p_islandsToCheck.push_back(*subIt);
//                }
//            }
//            
//            if (holes.size() == 0) {
//                for (vector<shared_ptr<Island>>::iterator it = searchSpace.begin(); it != searchSpace.end(); it++) {
//                    
//                }
//            } else {
//                for (vector<vector<shared_ptr<Island>>::iterator>::iterator it = p_topLevelPolysIterators.begin(); it != searchSpace.end(); it++) {
//                    for (vector<shared_ptr<Island>>::iterator subIt = p_islandsToCheck.begin(); subIt != p_islandsToCheck.end(); subIt++) {
//                        //for (vector<Polygon>)
//                    }
//                }
//            }
//            
//            searchSpace = newSearchSpace;
//        }
//    }
    
    return pair<Slice, vector<shared_ptr<const MeshFace>>>(ret, p_intersectingFaces);
}
