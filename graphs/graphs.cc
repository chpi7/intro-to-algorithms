#include <iostream>
#include <sstream>
#include <string>
#include "graph.h"

using namespace ita::graphs;

int main() {
    std::cout << "Graphs\n";

    // Page 616, Figure 22.9
    std::vector<Edge> edges {
        {1, 2},
        {2, 3},
        {2, 6},
        {2, 5},
        {3, 4},
        {3, 7},
        {4, 3},
        {4, 8},
        {8, 8},
        {7, 8},
        {7, 6},
        {6, 7},
        {5, 6},
        {5, 1}
    };
    DGraph g{edges};

    std::stringstream ss1;
    g.to_dot(ss1, true);

    auto r = g.scc();
    for(auto& set: *r){
        for(auto id: set){
            std::cout << (char)(id + 0x60) << " ";
        }
        std::cout << "\n";
    }

    

    return 0;
}