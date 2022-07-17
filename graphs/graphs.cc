#include <iostream>
#include "graph.h"

using namespace ita::graphs;

int main() {
    std::cout << "Graphs\n";

    std::vector<Edge> edges {
        {2, 1},
        {1, 3},
        {3, 2},
        {4, 2},
        {4, 3},
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 6},
        {5, 8},
        {8, 4},
        {9, 10},
        {9, 8},
        {10, 8}
    };
    Graph g{edges};

    auto result = g.dfs();

    for(auto p: *result) {
        std::cout << "Node " << p->id << ":\t" << p->t_discover << " to " << p->t_finish << "\n";
    }

    return 0;
}