#pragma once

#include "QGraph/qnode.hh"
#include "QGraph/qsocket.hh"
#include "QGraph/qtypes.hh"
#include <memory>
#include <optional>
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
    nodes.back()->id = nodes.size() - 1;
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

  template <typename F>
  void connect(NodeId from_node, const SocketId at_out_socket, NodeId to_node,
               const SocketId at_in_socket) {

    std::shared_ptr<qgraph::OutSocket<F>> a =
        get_node(from_node)->get_output_socket<F>(at_out_socket).value();

    std::shared_ptr<qgraph::InSocket<F>> b =
        get_node(to_node)->get_input_socket<F>(at_in_socket).value();

    a->connect(to_node, b->id);
    b->connect(from_node, a->id);
  };

  void delete_node(qgraph::NodeId id) { nodes.erase(nodes.begin() + id); };

  std::shared_ptr<qgraph::Node> get_node(qgraph::NodeId id) const {
    return nodes[id];
  };

  template <typename T>
  std::optional<T> get_current_output_value(NodeId for_node,
                                            SocketId at_socket) {
    return nodes[for_node]
        ->get_output_socket<T>(at_socket)
        .value()
        ->get_current_value();
  };

  void propagate_values(NodeId for_node) const {
    for (auto [src_socket, dest_node, dest_socket] :
         nodes[for_node]->get_neighbors()) {

      auto source_node = get_node(for_node);
      auto dst_node = get_node(dest_node);

      switch (source_node->get_type_of_output_socket(src_socket)) {
      case INT: {
        dst_node->get_input_socket<int>(dest_socket)
            .value()
            ->set_current_value(source_node->get_output_socket<int>(src_socket)
                                    .value()
                                    ->get_current_value());
      }
      case BOOL:
        dst_node->get_input_socket<bool>(dest_socket)
            .value()
            ->set_current_value(source_node->get_output_socket<bool>(src_socket)
                                    .value()
                                    ->get_current_value());
      case STRING:
        dst_node->get_input_socket<std::string>(dest_socket)
            .value()
            ->set_current_value(
                source_node->get_output_socket<std::string>(src_socket)
                    .value()
                    ->get_current_value());
      }
    }
  };
};
}; // namespace qgraph
