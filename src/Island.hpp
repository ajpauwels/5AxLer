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
        const Polygon & mainPolygon() const;
        const std::vector<std::shared_ptr<const MeshFace>> & p_mainPolygonMeshFaces() const;
        const std::vector<Polygon> & holes() const;
        const std::vector<std::vector<std::shared_ptr<const MeshFace>>> & p_holesMeshFaces() const;
        const std::vector<std::vector<Island>> holeIslands() const;
        
        void addHole(Polygon hole, std::vector<std::shared_ptr<const MeshFace>> p_holeMeshFaces);
        void addIslandToHole(Island island, int holeIndex);
        
    private:
        Polygon mainPolygon_; //polygon that represents outline of island
        std::vector<std::shared_ptr<const MeshFace>> p_mainPolygonMeshFaces_; //ptr to MeshFace on each edge of mainPolygon_, i.e. p_mainPolygonMeshFaces_[x] is the MeshFace that the xth edge of mainPolygon_ came from
        
        std::vector<Polygon> holes_; //series of holes inside mainPolygon_
        std::vector<std::vector<std::shared_ptr<const MeshFace>>> p_holesMeshFaces_; //same as p_mainPolygonMeshFaces_ but p_holesMeshFaces_[i][x] is for the ith hole instead of the mainPolygon_
        std::vector<std::vector<Island>> holeIslands_; //islands inside each hole, so holeIslands_[i][j] is the jth island inside the ith hole
    };
}

#endif /* Island_hpp */
