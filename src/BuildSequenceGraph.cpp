//
//  BuildSequenceGraph.cpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 12/8/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#include "BuildSequenceGraph.hpp"

using namespace mapmqp;
using namespace std;

BuildSequenceGraph::BuildSequenceGraph() { }

const vector<shared_ptr<Mesh>> & BuildSequenceGraph::p_meshes() const {
    return m_p_meshes;
}

const vector<int> & BuildSequenceGraph::adjacencyLists(unsigned int index) const {
    if (index >= m_adjacencyLists.size()) {
        writeLog(WARNING, "attempted to access adjacency list of index out of range");
        static vector<int> emptyVector;
        return emptyVector;
    }
    
    return m_adjacencyLists[index];
}

const vector<int> & BuildSequenceGraph::baseAdjacencyLists(unsigned int index) const {
    if (index >= m_baseAdjacencyLists.size()) {
        writeLog(WARNING, "attempted to access base adjacency list of index out of range");
        static vector<int> emptyVector;
        return emptyVector;
    }
    
    return m_baseAdjacencyLists[index];
}

const vector<pair<int, Vector3D>> & BuildSequenceGraph::collisionAdjacencyLists(unsigned int index) const {
    if (index >= m_collisionAdjacencyLists.size()) {
        writeLog(WARNING, "attempted to access base adjacency list of index out of range");
        static vector<pair<int, Vector3D>> emptyVector;
        return emptyVector;
    }
    
    return m_collisionAdjacencyLists[index];
}

int BuildSequenceGraph::addVertex(shared_ptr<Mesh> p_mesh) {
    int index = m_p_meshes.size();
    m_p_meshes.push_back(p_mesh);
    m_baseAdjacencyLists.resize(index + 1);
    m_collisionAdjacencyLists.resize(index + 1);
    return index;
}

void BuildSequenceGraph::addBaseEdge(unsigned int sourceIndex, unsigned int destIndex) {
    if ((sourceIndex >= m_p_meshes.size()) || (destIndex >= m_p_meshes.size())) {
        writeLog(ERROR, "attempted to add edge between vertices not in range");
        return;
    }
    
    m_adjacencyLists[sourceIndex].push_back(destIndex);
    m_baseAdjacencyLists[sourceIndex].push_back(destIndex);
}

void BuildSequenceGraph::addCollisionEdge(unsigned int sourceIndex, unsigned int destIndex, Vector3D buildDirection) {
    if ((sourceIndex >= m_p_meshes.size()) || (destIndex >= m_p_meshes.size())) {
        writeLog(ERROR, "attempted to add edge between vertices not in range");
        return;
    }
    
    m_adjacencyLists[sourceIndex].push_back(destIndex);
    m_collisionAdjacencyLists[sourceIndex].push_back(pair<int, Vector3D>(destIndex, buildDirection));
}

vector<vector<int>> BuildSequenceGraph::findCycles() const {
    //detects cycles using Tarjan's strongly connected components algorithm, see http://www.geeksforgeeks.org/tarjan-algorithm-find-strongly-connected-components/
    
    vector<vector<int>> cycles;
    
    int elementCount = m_p_meshes.size();
    int discCount = 0;
    vector<int> discs(elementCount, -1);
    vector<int> lows(elementCount, -1);
    stack<int> stack;
    vector<bool> inStack(elementCount, false);
    
    for (unsigned int i = 0; i < elementCount; i++) {
        if (discs[i] == -1) {
            findCyclesUtil(i, discCount, discs, lows, stack, inStack, cycles);
        }
    }
    
    return cycles;
}

void BuildSequenceGraph::findCyclesUtil(int index, int & discCount, vector<int> & discs, vector<int> & lows, stack<int> & stack, vector<bool> & inStack, vector<vector<int>> & cycles) const {
    discs[index] = lows[index] = discCount++;
    stack.push(index);
    inStack[index] = true;
    
    for (vector<int>::const_iterator it = m_adjacencyLists[index].begin(); it != m_adjacencyLists[index].end(); it++) {
        int otherIndex = *it;
        
        if (discs[otherIndex] == -1) {
            findCyclesUtil(otherIndex, discCount, discs, lows, stack, inStack, cycles);
            lows[index] = fmin(lows[index], lows[otherIndex]);
        } else if (inStack[otherIndex]) {
            lows[index] = fmin(lows[index], discs[otherIndex]);
        }
    }
    
    if (lows[index] == discs[index]) { //head node found
        vector<int> cycle;
        
        int otherIndex = -1;
        do {
            otherIndex = stack.top();
            stack.pop();
            
            inStack[otherIndex] = false;
            cycle.push_back(otherIndex);
        } while (otherIndex != index);
        
        if (cycle.size() > 1) {
            cycles.push_back(cycle);
        }
    }
}

stack<int> BuildSequenceGraph::topologicalSort() const {
    //see http://www.geeksforgeeks.org/topological-sorting/
    stack<int> sortedGraph;
    
    int elementCount = m_p_meshes.size();
    vector<bool> visited(elementCount, false);
    
    for (unsigned int i = 0; i < elementCount; i++) {
        if (!visited[i]) {
            topologicalSortUtil(i, visited, sortedGraph);
        }
    }
    
    return sortedGraph;
}

void BuildSequenceGraph::topologicalSortUtil(int index, vector<bool> & visited, stack<int> & sortedGraph) const {
    visited[index] = true;
    
    for (vector<int>::const_iterator it = m_adjacencyLists[index].begin(); it != m_adjacencyLists[index].end(); it++) {
        if (!visited[*it]) {
            topologicalSortUtil(*it, visited, sortedGraph);
        }
    }
    
    sortedGraph.push(index);
}
