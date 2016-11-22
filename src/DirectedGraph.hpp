//
//  DirectedGraph.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 11/21/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef DirectedGraph_hpp
#define DirectedGraph_hpp

#include <memory>
#include <vector>
#include <stack>

#include "Utility.hpp"

namespace mapmqp {
    template<typename T>
    class DirectedGraph {
    public:
        DirectedGraph();
        
        const std::vector<T> & elements() const;
        const std::vector<int> & adjacentyList(unsigned int index) const;
        
        //returns index of element in graph
        int addVertex(T element);
        void addDirectedEdge(int sourceIndex, int destIndex);
        
        std::vector<std::vector<int>> findCycles() const;
        std::stack<int> topologicalSort() const; //if cycles exist, this will return an topological sort although invalid
        
    private:
        std::vector<T> m_elements;
        std::vector<std::vector<int>> m_adjacencyLists;
        
        void findCyclesUtil(int index, int & discCount, std::vector<int> & discs, std::vector<int> & lows, std::stack<int> & stack, std::vector<bool> & inStack, std::vector<std::vector<int>> & cycles) const;
        void topologicalSortUtil(int index, std::vector<bool> & visited, std::stack<int> & sortedGraph) const;
    };
    
    //function definitions
    
    template<typename T>
    DirectedGraph<T>::DirectedGraph() { }
    
    template<typename T>
    const std::vector<T> & DirectedGraph<T>::elements() const {
        return m_elements;
    }
    
    template<typename T>
    const std::vector<int> & DirectedGraph<T>::adjacentyList(unsigned int index) const {
        if (index >= m_adjacencyLists.size()) {
            writeLog(WARNING, "attempted to access adjacency list of index out of range");
            static std::vector<int> emptyVector;
            return emptyVector;
        }
        
        return m_adjacencyLists[index];
    }
    
    template<typename T>
    int DirectedGraph<T>::addVertex(T element) {
        int index = m_elements.size();
        m_elements.push_back(element);
        m_adjacencyLists.resize(index + 1);
        return index;
    }
    
    template<typename T>
    void DirectedGraph<T>::addDirectedEdge(int sourceIndex, int destIndex) {
        if ((sourceIndex >= m_adjacencyLists.size()) || (destIndex >= m_adjacencyLists.size())) {
            writeLog(ERROR, "attempted to add edge between vertices not in range");
            return;
        }
        
        m_adjacencyLists[sourceIndex].push_back(destIndex);
    }
    
    template<typename T>
    std::vector<std::vector<int>> DirectedGraph<T>::findCycles() const {
        //detects cycles using Tarjan's strongly connected components algorithm, see http://www.geeksforgeeks.org/tarjan-algorithm-find-strongly-connected-components/
        
        std::vector<std::vector<int>> cycles;
        
        int elementCount = m_elements.size();
        int discCount = 0;
        std::vector<int> discs(elementCount, -1);
        std::vector<int> lows(elementCount, -1);
        std::stack<int> stack;
        std::vector<bool> inStack(elementCount, false);
        
        for (unsigned int i = 0; i < elementCount; i++) {
            if (discs[i] == -1) {
                findCyclesUtil(i, discCount, discs, lows, stack, inStack, cycles);
            }
        }
        
        return cycles;
    }
    
    template<typename T>
    void DirectedGraph<T>::findCyclesUtil(int index, int & discCount, std::vector<int> & discs, std::vector<int> & lows, std::stack<int> & stack, std::vector<bool> & inStack, std::vector<std::vector<int>> & cycles) const {
        discs[index] = lows[index] = discCount++;
        stack.push(index);
        inStack[index] = true;
        
        for (std::vector<int>::const_iterator it = m_adjacencyLists[index].begin(); it != m_adjacencyLists[index].end(); it++) {
            int otherIndex = *it;
            
            if (discs[otherIndex] == -1) {
                findCyclesUtil(otherIndex, discCount, discs, lows, stack, inStack, cycles);
                lows[index] = fmin(lows[index], lows[otherIndex]);
            } else if (inStack[otherIndex]) {
                lows[index] = fmin(lows[index], discs[otherIndex]);
            }
        }
        
        if (lows[index] == discs[index]) { //head node found
            std::vector<int> cycle;
            
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
    
    template<typename T>
    std::stack<int> DirectedGraph<T>::topologicalSort() const {
        //see http://www.geeksforgeeks.org/topological-sorting/
        std::stack<int> sortedGraph;
        
        int elementCount = m_elements.size();
        std::vector<bool> visited(elementCount, false);
        
        for (unsigned int i = 0; i < elementCount; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, sortedGraph);
            }
        }
        
        return sortedGraph;
    }
    
    template<typename T>
    void DirectedGraph<T>::topologicalSortUtil(int index, std::vector<bool> & visited, std::stack<int> & sortedGraph) const {
        visited[index] = true;
        
        for (std::vector<int>::const_iterator it = m_adjacencyLists[index].begin(); it != m_adjacencyLists[index].end(); it++) {
            if (!visited[*it]) {
                topologicalSortUtil(*it, visited, sortedGraph);
            }
        }
        
        sortedGraph.push(index);
    }
}

#endif /* DirectedGraph_hpp */
