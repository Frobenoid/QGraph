#pragma once

#include "QGraph/qnode.hh"
#include "QGraph/qsocket.hh"
#include <memory>
#include <type_traits>
#include <vector>

namespace qgraph {
template <typename T>
concept ConceptObject = std::is_base_of<qgraph::Node, T>::value;

class Graph {
public:
  std::vector<std::shared_ptr<qgraph::Node>> nodes;

  template <ConceptObject T> void add_node() {
    nodes.emplace_back(std::make_shared<T>());
  };

  template <typename F>
  void connect(qgraph::NodeId from_node, const std::string &at_out_socket,
               qgraph::NodeId to_node, const std::string &at_in_socket) {

    std::shared_ptr<qgraph::OutSocket<F>> a =
        get_node(from_node)->get_output_socket<F>(at_out_socket).value();

    std::shared_ptr<qgraph::InSocket<F>> b =
        get_node(to_node)->get_input_socket<F>(at_in_socket).value();

    a->connect(to_node, b->id);
    b->connect(from_node, a->id);
  };

  void delete_node(qgraph::NodeId id) { nodes.erase(nodes.begin() + id); };

  std::shared_ptr<qgraph::Node> get_node(qgraph::NodeId id) {
    return nodes[id];
  };
};
}; // namespace qgraph
