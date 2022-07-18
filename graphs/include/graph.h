#ifndef ITA_GRAPH_GRAPH_H
#define ITA_GRAPH_GRAPH_H

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <list>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace ita { namespace graphs {

typedef int id_t;

enum VColor { WHITE, GRAY, BLACK };

struct Vertex {
    id_t id;

    VColor color;
    Vertex* parent;

    // DFS discovery time.
    unsigned t_discover;
    // DFS finish time.
    unsigned t_finish;

    bool operator==(const Vertex &other) const { return id == other.id; }

public:
    Vertex(id_t id): id(id) {};
    Vertex(const Vertex&) = delete;
    Vertex& operator=(Vertex const&) = delete;
};

// A (possibly directed) weighted edge: from -> / <-> to.
struct Edge {
    id_t from;
    id_t to;
    int weight;

public:
    Edge(id_t from, id_t to, int weight = 0): from(from), to(to), weight(weight) {};
};

}}

namespace std {
  template <>
  struct hash<ita::graphs::Vertex>
  {
    std::size_t operator()(const ita::graphs::Vertex& k) const { return k.id; };
  };
}

namespace ita { namespace graphs {

// A directed graph.
class DGraph {

private:
    // used in various algorithms.
    unsigned time;
    // Vertices of G.
    std::unordered_map<id_t, Vertex> vertices;
    // Adjacency list of G.
    std::unordered_map<id_t, std::vector<Edge>> edges;

public:

    DGraph(std::vector<Edge>& _edges) {
        for(auto& edge: _edges){
            vertices.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(edge.from),
                std::forward_as_tuple(edge.from)
            );
            vertices.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(edge.to),
                std::forward_as_tuple(edge.to)
            );
            edges[edge.from].push_back(edge);
        }
    };

    void to_dot(std::ostream& stream, bool id_as_char = false) const {
        stream << "digraph {\n";

        for(auto& [_, edge_list]: edges){
            for(auto& edge: edge_list) {
                if(id_as_char){
                    stream << "  " << (char)(edge.from + 0x60) << " -> " << (char)(edge.to + 0x60);
                } else {
                    stream << "  " << edge.from << " -> " << edge.to;
                }
                if(edge.weight > 0) {
                    stream << " [label=" << edge.weight << "]";
                }
                stream << ";\n";
            }
        }

        stream << "}\n";
    }

    // Transpose
    std::unique_ptr<DGraph> transpose() {
        std::vector<Edge> edges;
        for(auto& [_, edge_list]: this->edges){
            for(auto& edge: edge_list){
                edges.emplace_back(edge.to, edge.from, edge.weight);
            }
        }
        return std::make_unique<DGraph>(edges);
    }

    // Performs DFS on the graph, updates the vertex attributes, 
    // and returns the list of vertices ordered by finishing time (topologically sorted).
    std::unique_ptr<std::list<Vertex*>> dfs(bool topological_order = true) {

        std::unique_ptr<std::list<Vertex*>> result = std::make_unique<std::list<Vertex*>>();

        for(auto& p: vertices){
            p.second.color = WHITE;
            p.second.parent = nullptr;
            p.second.t_discover = 0;
            p.second.t_finish = 0;
        }

        time = 0;
        for(auto& [_, v]: vertices){
            if(v.color == WHITE) {
                auto r = dfs_visit(&v);
                result->splice(result->end(), *r);
            }
        }

        if(topological_order){
            // Last finished should be first, first finished should be last to get topological order.
            result->reverse();
        }
        return result;
    }

    std::unique_ptr<std::vector<std::unordered_set<id_t>>> dfs_forest(std::vector<id_t>& node_order) {

        auto result = std::make_unique<std::vector<std::unordered_set<id_t>>>();

        for(auto& p: vertices){
            p.second.color = WHITE;
            p.second.parent = nullptr;
            p.second.t_discover = 0;
            p.second.t_finish = 0;
        }

        time = 0;
        for(auto node_id: node_order){
            Vertex& v = vertices.at(node_id);
            if(v.color == WHITE) {
                auto r = dfs_visit(&v);
                result->emplace_back();
                for(auto vp: *r){
                    result->back().insert(vp->id);
                }
            }
        }

        return result;
    }

    // DFS visit from one vertex. Does not initialize any attributes.
    std::unique_ptr<std::list<Vertex*>> dfs_visit(Vertex* root){
        std::list<Vertex*> stack;
        std::unique_ptr<std::list<Vertex*>> result = std::make_unique<std::list<Vertex*>>();
        std::unordered_set<id_t> seen_nodes;
        
        stack.push_back(root);

        while(!stack.empty()){
            Vertex* u = stack.back();

            switch(u->color) {
                case WHITE: 
                    u->color = GRAY;
                    u->t_discover = ++time;
                    for(auto& edge: edges[u->id]){
                        Vertex* v = &vertices.at(edge.to);
                        if(v->color == WHITE && !seen_nodes.contains(v->id)) {
                            v->parent = u;
                            seen_nodes.insert(v->id);
                            stack.push_back(v);
                        }
                    }
                    break;
                case GRAY:
                    // all children of u are done processing.
                    u->t_finish = ++time;
                    u->color = BLACK; // this is kinda unnecessary but hey.
                    stack.pop_back();
                    result->push_back(u);
                    break;
                case BLACK:
                default:
                    throw std::runtime_error("Invalid / Unexpected node color");
            }
        }

        return result;
    }

    // Compute strongly connected components.
    std::unique_ptr<std::vector<std::unordered_set<int>>> scc(){

        // Contains the nodes ordered by finishing time in ascending order.
        std::unique_ptr<std::list<Vertex*>> forward_result = dfs(true);

        // Run DFS again on the transposed graph, in topological order.
        std::unique_ptr<DGraph> transposed_graph = transpose();
        std::vector<id_t> node_order;
        for(auto vp: *forward_result) node_order.push_back(vp->id);

        return transposed_graph->dfs_forest(node_order);
    }
};

}}

#endif