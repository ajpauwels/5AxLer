//
//  Island.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/4/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef Island_hpp
#define Island_hpp

#include <vector>

#include "Polygon.hpp"

namespace mapmqp {
    class MeshFace;
    
    class Island {
    public:
        struct Hole {
            friend class Island;
            
        public:
            Hole(Polygon holePolygon, std::vector<std::shared_ptr<const MeshFace>> p_holeMeshFaces) :
            holePolygon_(holePolygon),
            p_holeMeshFaces_(p_holeMeshFaces) { }
            
            std::shared_ptr<const Island> p_parentIsland() { return p_parentIsland_; }
            const Polygon & holePolygon() { return holePolygon_; }
            const std::vector<std::shared_ptr<const MeshFace>> & p_holeMeshFaces() { return p_holeMeshFaces_; }
            const std::vector<std::shared_ptr<const Island>> & p_holeIslands() { return p_holeIslands_; }
            
        private:
            std::shared_ptr<const Island> p_parentIsland_ = nullptr;
            Polygon holePolygon_;
            std::vector<std::shared_ptr<const MeshFace>> p_holeMeshFaces_;
            std::vector<std::shared_ptr<const Island>> p_holeIslands_;
        };
        
        Island(const Polygon & mainPolygon, std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces);
        
        //getters
        const Polygon & mainPolygon() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_mainPolygonMeshFaces() const;
        const std::vector<std::shared_ptr<Hole>> & holes() const;
        
        void addHole(std::shared_ptr<Hole> p_hole);
        void addIslandToHole(std::shared_ptr<const Island> p_island, unsigned int holeIndex);
        
        //returns all islands at a certain depth (0 is this)
        std::vector<std::shared_ptr<const Island>> getP_SubIslandsAtDepth(unsigned int depth);
        
    private:
        Polygon mainPolygon_; //polygon that represents outline of island
        std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces_; //ptr to MeshFace on each edge of mainPolygon_, i.e. p_mainPolygonMeshFaces_[x] is the MeshFace that the xth edge of mainPolygon_ came from
        
        std::vector<std::shared_ptr<Hole>> holes_;
    };
}

#endif /* Island_hpp */
