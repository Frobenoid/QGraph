#pragma once

#include "QGraph/qnode.hh"
#include "QGraph/qsocket.hh"
#include <iostream>
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
      // // Retrieve destination node.
      // auto dest = get_node(dest_node);
      // auto src_node = get_node(for_node);
      //
      // // Retrieve destination INPUT socket.
      // auto dst_socket =
      //     dest->get_input_socket<typeof(dest->in_sockets_[dest_socket])>(
      //             dest_socket)
      //         .value();
      //
      // std::cout << typeid(dst_socket).name() << '\n';
      //
      // auto source_socket =
      //     src_node
      //         ->get_output_socket<typeof(src_node->out_sockets_[src_socket])>(
      //             src_socket)
      //         .value();
      //
      // // Propagate value
      // auto sval = source_socket->get_current_value();
      // // dst_socket->set_current_value(source_socket->get_current_value());
    }
  };
};
}; // namespace qgraph
