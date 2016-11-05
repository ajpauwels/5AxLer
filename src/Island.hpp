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
        Island(const Polygon & mainPolygon, std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces);
        
        //getters
        const Polygon & mainPolygon();
        const std::vector<std::shared_ptr<const MeshFace>> & p_mainPolygonMeshFaces();
        const std::vector<Polygon> & holes();
        const std::vector<std::vector<std::shared_ptr<const MeshFace>>> & p_holesMeshFaces();
        
        void addHole(Polygon hole, std::vector<std::shared_ptr<const MeshFace>> p_holeMeshFaces);
        
    private:
        Polygon mainPolygon_;
        std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces_;
        
        std::vector<Polygon> holes_;
        std::vector<std::vector<std::shared_ptr<const MeshFace>>> p_holesMeshFaces_;
    };
}

#endif /* Island_hpp */
