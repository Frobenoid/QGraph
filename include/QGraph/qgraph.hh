#pragma once

#include "QGraph/qnode.hh"
#include "QGraph/qsocket.hh"
#include "QGraph/qtypes.hh"
#include <algorithm>
#include <cassert>
#include <memory>
#include <optional>
#include <sys/types.h>
#include <type_traits>
#include <utility>
#include <vector>

namespace qgraph {
template <typename T>
concept DerivesNode = std::is_base_of<qgraph::Node, T>::value;

class Graph {
public:
  std::vector<std::shared_ptr<qgraph::Node>> nodes;

  template <DerivesNode T, typename... Args> void add_node(Args... args) {
    nodes.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
    nodes.back()->set_id(nodes.size() - 1);
  };

  template <typename F>
  void connect(qgraph::NodeId from_node, const std::string &at_out_socket,
               qgraph::NodeId to_node, const std::string &at_in_socket) {

    assert(from_node < nodes.size());
    assert(to_node < nodes.size());

    std::shared_ptr<qgraph::OutSocket<F>> a =
        get_node(from_node)->get_output_socket<F>(at_out_socket).value();

    std::shared_ptr<qgraph::InSocket<F>> b =
        get_node(to_node)->get_input_socket<F>(at_in_socket).value();

    a->connect(to_node, b->id());
    b->connect(from_node, a->id);
  };

  template <typename F>
  void connect(NodeId from_node, const SocketId at_out_socket, NodeId to_node,
               const SocketId at_in_socket) {

    assert(from_node < nodes.size());
    assert(to_node < nodes.size());

    auto a = get_node(from_node);
    auto a_socket = a->get_output_socket<F>(at_out_socket).value();

    assert(at_out_socket < a->num_of_output_sockets());

    auto b = get_node(to_node);
    auto b_socket = b->get_input_socket<F>(at_in_socket).value();

    assert(at_in_socket < b->num_of_input_sockets());

    a_socket->connect(to_node, b_socket->id());
    b_socket->connect(from_node, a_socket->id);
  };

  void delete_node(qgraph::NodeId id) { nodes.erase(nodes.begin() + id); };

  std::shared_ptr<qgraph::Node> get_node(qgraph::NodeId id) const {
    return nodes[id];
  };

  int get_number_of_nodes() { return nodes.size(); }

  //
  // Socket access from graph.
  //

  // FIX: This should not be optional. If the socket does not
  // exists then it should return an error.
  template <typename T>
  std::optional<T> get_current_output_value(NodeId for_node,
                                            SocketId at_socket) const {
    return nodes[for_node]
        ->get_output_socket<T>(at_socket)
        .value()
        ->get_current_value();
  };

  // FIX: This should not be optional. If the socket does not
  // exists then it should return an error.
  template <typename T>
  std::optional<T> get_default_output_value(NodeId for_node,
                                            SocketId at_socket) const {
    return nodes[for_node]
        ->get_output_socket<T>(at_socket)
        .value()
        ->get_default_value();
  };

  template <typename T>
  void set_current_output_value(NodeId for_node, SocketId at_socket, T to) {
    nodes[for_node]->get_output_socket<T>(at_socket).value()->set_current_value(
        to);
  };

  template <typename T>
  void set_default_output_value(NodeId for_node, SocketId at_socket, T to) {
    nodes[for_node]->get_output_socket<T>(at_socket).value()->set_default_value(
        to);
  };

  // FIX: This should not be optional. If the socket does not
  // exists then it should return an error.
  template <typename T>
  std::optional<T> get_current_input_value(NodeId for_node,
                                           SocketId at_socket) const {
    return nodes[for_node]
        ->get_input_socket<T>(at_socket)
        .value()
        ->get_current_value();
  };

  // FIX: This should not be optional. If the socket does not
  // exists then it should return an error.
  template <typename T>
  std::optional<T> get_default_input_value(NodeId for_node,
                                           SocketId at_socket) const {
    return nodes[for_node]
        ->get_input_socket<T>(at_socket)
        .value()
        ->get_default_value();
  };

  template <typename T>
  void set_current_input_value(NodeId for_node, SocketId at_socket, T to) {
    nodes[for_node]->get_input_socket<T>(at_socket).value()->set_current_value(
        to);
  };

  template <typename T>
  void set_default_input_value(NodeId for_node, SocketId at_socket, T to) {
    nodes[for_node]->get_input_socket<T>(at_socket).value()->set_default_value(
        to);
  };

  void propagate_values(NodeId for_node) const {
    // TODO: Currently this function only works if
    // the source and destination sockets are of the same
    // type. It would be a good idea to make this work also
    // if they have types A and B such that A can be casted
    // into B.
    auto neighbors = get_node(for_node)->get_neighbors();

    std::ranges::for_each(neighbors, [this, for_node](const auto &link) {
      auto [src_socket, dst_node, dst_socket] = link;

      auto source_node = get_node(for_node);
      auto dest_node = get_node(dst_node);
      auto output_socket = source_node->get_untyped_output_socket(src_socket);
      auto input_socket = dest_node->get_untyped_input_socket(dst_socket);

      auto src_value = output_socket->get_untyped_current_value();
      input_socket->set_current_value(src_value);
    });
  };
};
}; // namespace qgraph
