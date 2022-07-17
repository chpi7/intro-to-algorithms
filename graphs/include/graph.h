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

class Graph {

private:
    // used in various algorithms.
    unsigned time;
    // Vertices of G.
    std::unordered_map<id_t, Vertex> vertices;
    // Adjacency list of G.
    std::unordered_map<id_t, std::vector<Edge>> edges;

public:

    Graph(std::vector<Edge>& _edges) {
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

    // Performs DFS on the graph, updates the vertex attributes, 
    // and returns the list of vertices ordered by finishing time (topologically sorted).
    std::unique_ptr<std::list<Vertex*>> dfs() {

        std::unique_ptr<std::list<Vertex*>> result = std::make_unique<std::list<Vertex*>>();

        for(auto& p: vertices){
            p.second.color = WHITE;
            p.second.parent = nullptr;
            p.second.t_discover = 0;
            p.second.t_finish = 0;
        }

        time = 0;
        for(auto& p: vertices){
            if(p.second.color == WHITE) {
                auto r = dfs_visit(p.second);
                result->splice(result->end(), *r);
            }
        }

        return result;
    }

    // DFS visit from one vertex. Does not initialize any attributes.
    std::unique_ptr<std::list<Vertex*>> dfs_visit(Vertex& root){
        std::list<Vertex*> stack;
        std::unique_ptr<std::list<Vertex*>> result = std::make_unique<std::list<Vertex*>>();
        std::unordered_set<id_t> seen_nodes;
        
        stack.push_back(&root);

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
};

}}

#endif