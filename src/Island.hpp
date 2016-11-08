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
#include <memory>

#include "Polygon.hpp"

namespace mapmqp {
    class MeshFace;
    
    class Island {
    public:        
        Island(const Polygon & mainPolygon, std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces, bool isHole = false);
        
        // Getters
        const Polygon & polygon() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_mainPolygonMeshFaces() const;
        const std::vector<std::shared_ptr<Island>> & children() const;
        
        void addChild(std::shared_ptr<Island> p_child);

        std::shared_ptr<const Island> m_p_parentIsland;
        
    private:
        Polygon m_polygon; //polygon that represents outline of island
        std::vector<std::shared_ptr<const MeshFace>> m_p_mainPolygonMeshFaces; //ptr to MeshFace on each edge of mainPolygon_, i.e. p_mainPolygonMeshFaces_[x] is the MeshFace that the xth edge of mainPolygon_ came from
        
        std::vector<std::shared_ptr<Island>> m_children;

        bool m_isHole;
    };
}

#endif /* Island_hpp */
