//
//  BuildSequenceGraphToGraphviz.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 12/8/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "BuildSequenceGraphToGraphviz.hpp"

using namespace mapmqp;
using namespace std;

static bool parse(std::string filePath, const BuildSequenceGraph & buildSequenceGraph) {
    ofstream file;
    file.open(filePath, ios::out | ios::binary);
    
    if (file.is_open()) {
        file << "digraph {\n";
        
        for (unsigned int i = 0; i < buildSequenceGraph.p_meshes().size(); i++) {
            for (auto & adj : buildSequenceGraph.baseAdjacencyLists(i)) {
                file << i << " -> " << adj << ";\n";
            }
        }
        
        file << "edge [color=red];\n";
        
        for (unsigned int i = 0; i < buildSequenceGraph.p_meshes().size(); i++) {
            for (auto & adj : buildSequenceGraph.collisionAdjacencyLists(i)) {
                file << i << " -> " << adj.first << ";\n";
            }
        }
        
        file << "}";
        file.close();
        
        return true;
    } else {
        writeLog(WARNING, "could not open MATLAB script file");
        return false;
    }
}
