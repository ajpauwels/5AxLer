//
//  BuildSequenceGraph.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 12/8/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef BuildSequenceGraph_hpp
#define BuildSequenceGraph_hpp

#include <memory>
#include <vector>
#include <stack>

#include "Utility.hpp"
#include "Mesh.hpp"

namespace mapmqp {
    class BuildSequenceGraph {
    public:
        BuildSequenceGraph();
        
        const std::vector<std::shared_ptr<Mesh>> & p_meshes() const;
        const std::vector<int> & adjacencyLists(unsigned int index) const;
        const std::vector<int> & baseAdjacencyLists(unsigned int index) const;
        const std::vector<std::pair<int, Vector3D>> & collisionAdjacencyLists(unsigned int index) const;
        
        //returns index of element in graph
        int addVertex(std::shared_ptr<Mesh> p_mesh);
        void addBaseEdge(unsigned int sourceIndex, unsigned int destIndex);
        void addCollisionEdge(unsigned int sourceIndex, unsigned int destIndex, Vector3D buildDirection);
        
        std::vector<std::vector<int>> findCycles() const;
        std::stack<int> topologicalSort() const; //if cycles exist, this will return an topological sort although invalid
        
    protected:
        std::vector<std::shared_ptr<Mesh>> m_p_meshes;
        std::vector<std::vector<int>> m_adjacencyLists;
        std::vector<std::vector<int>> m_baseAdjacencyLists;
        std::vector<std::vector<std::pair<int, Vector3D>>> m_collisionAdjacencyLists;
        
        void findCyclesUtil(int index, int & discCount, std::vector<int> & discs, std::vector<int> & lows, std::stack<int> & stack, std::vector<bool> & inStack, std::vector<std::vector<int>> & cycles) const;
        void topologicalSortUtil(int index, std::vector<bool> & visited, std::stack<int> & sortedGraph) const;
    };
}

#endif /* BuildSequenceGraph_hpp */
