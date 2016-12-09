//
//  BuildSequenceGraphToGraphviz.hpp
//  5AxLer
//
//  Created by Ethan Coeytaux on 12/8/16.
//  Copyright © 2016 MAP MQP. All rights reserved.
//

#ifndef BuildSequenceGraphToGraphviz_hpp
#define BuildSequenceGraphToGraphviz_hpp

#include "BuildSequenceGraph.hpp"

namespace mapmqp {
    class BuildSequenceGraphToGraphviz {
        static bool parse(std::string filePath, const BuildSequenceGraph & buildSequenceGraph);
    };
}

#endif /* BuildSequenceGraphToGraphviz_hpp */
