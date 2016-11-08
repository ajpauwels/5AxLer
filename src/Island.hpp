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
            Hole(Polygon polygon, std::vector<std::shared_ptr<const MeshFace>> p_meshFaces) :
            m_polygon(polygon),
            m_p_meshFaces(p_meshFaces) { }
            
            std::shared_ptr<const Island> p_parentIsland() { return m_p_parentIsland; }
            const Polygon & polygon() { return m_polygon; }
            const std::vector<std::shared_ptr<const MeshFace>> & p_meshFaces() { return m_p_meshFaces; }
            const std::vector<std::shared_ptr<const Island>> & p_islands() { return m_p_islands; }
            
        private:
            std::shared_ptr<const Island> m_p_parentIsland = nullptr;
            Polygon m_polygon;
            std::vector<std::shared_ptr<const MeshFace>> m_p_meshFaces;
            std::vector<std::shared_ptr<const Island>> m_p_islands;
        };
        
        Island(const Polygon & mainPolygon, std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces);
        
        //getters
        const Polygon & polygon() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_mainPolygonMeshFaces() const;
        const std::vector<std::shared_ptr<Hole>> & holes() const;
        
        void addHole(std::shared_ptr<Hole> p_hole);
        void addIslandToHole(std::shared_ptr<const Island> p_island, unsigned int holeIndex);
        
        //returns all islands at a certain depth (0 is this)
        std::vector<std::shared_ptr<const Island>> getP_SubIslandsAtDepth(unsigned int depth);
        
    private:
        Polygon m_polygon; //polygon that represents outline of island
        std::vector<std::shared_ptr<const MeshFace>> m_p_mainPolygonMeshFaces; //ptr to MeshFace on each edge of mainPolygon_, i.e. p_mainPolygonMeshFaces_[x] is the MeshFace that the xth edge of mainPolygon_ came from
        
        std::vector<std::shared_ptr<Hole>> m_holes;
    };
}

#endif /* Island_hpp */
