#pragma once

#include "QGraph/qnode.hh"
#include "QGraph/qsocket.hh"
#include "QGraph/qtypes.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <sys/types.h>
#include <type_traits>
#include <utility>
#include <vector>

namespace qgraph {

template <typename T>
concept DerivesNode = std::is_base_of<qgraph::Node, T>::value;

class Graph {
private:
  std::vector<std::shared_ptr<qgraph::Node>> nodes_;

public:
  size_t num_of_nodes() const { return nodes_.size(); }

  void execute_node(NodeId node) {
    if (node < nodes_.size()) {
      this->node(node)->execute();
    } else {
      throw std::out_of_range("Node ID is out of range.");
    }
  }

  template <DerivesNode T, typename... Args> void add_node(Args... args) {
    nodes_.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
    nodes_.back()->set_id(nodes_.size() - 1);
  };

  template <typename F>
  [[deprecated("Socket labels will be removed")]]
  void
  connect_deprecated(qgraph::NodeId from_node, const std::string &at_out_socket,
                     qgraph::NodeId to_node, const std::string &at_in_socket) {

    assert(from_node < nodes_.size());
    assert(to_node < nodes_.size());

    std::shared_ptr<qgraph::OutSocket<F>> a =
        node(from_node)->output_socket<F>(at_out_socket).value();

    std::shared_ptr<qgraph::InSocket<F>> b =
        node(to_node)->input_socket<F>(at_in_socket).value();

    a->connect(to_node, b->id());
    b->connect(from_node, a->id());
  };

  template <typename F>
  void connect(NodeId from_node, const SocketId at_out_socket, NodeId to_node,
               const SocketId at_in_socket) {

    assert(from_node < nodes_.size());
    assert(to_node < nodes_.size());

    auto a = node(from_node);
    auto a_socket = a->output_socket<F>(at_out_socket);

    assert(at_out_socket < a->num_of_output_sockets());

    auto b = node(to_node);
    auto b_socket = b->input_socket<F>(at_in_socket);

    assert(at_in_socket < b->num_of_input_sockets());

    a_socket->connect(to_node, b_socket->id());
    b_socket->connect(from_node, a_socket->id());
  };

  // TODO: Does this invalidate ids? Write a test for it.
  // This can be achieved by using index masks.
  void delete_node(qgraph::NodeId id) { nodes_.erase(nodes_.begin() + id); };

  std::shared_ptr<qgraph::Node> node(qgraph::NodeId id) const {
    return nodes_[id];
  };

  //
  // Socket access from graph.
  //

  template <typename T>
  T current_output_value(NodeId for_node, SocketId at_socket) const {
    return nodes_[for_node]->output_socket<T>(at_socket)->current_value();
  };

  template <typename T>
  T default_output_value(NodeId for_node, SocketId at_socket) const {
    return nodes_[for_node]->output_socket<T>(at_socket)->default_value();
  };

  template <typename T>
  T current_input_value(NodeId for_node, SocketId at_socket) const {
    return nodes_[for_node]->input_socket<T>(at_socket)->current_value();
  };

  template <typename T>
  T default_input_value(NodeId for_node, SocketId at_socket) const {
    return nodes_[for_node]->input_socket<T>(at_socket)->get_default_value();
  };

  template <typename T>
  void set_current_output_value(NodeId for_node, SocketId at_socket, T to) {
    nodes_[for_node]->output_socket<T>(at_socket)->set_current_value(to);
  };

  template <typename T>
  void set_default_output_value(NodeId for_node, SocketId at_socket, T to) {
    nodes_[for_node]->output_socket<T>(at_socket)->set_default_value(to);
  };

  template <typename T>
  void set_current_input_value(NodeId for_node, SocketId at_socket, T to) {
    nodes_[for_node]->input_socket<T>(at_socket)->set_current_value(to);
  };

  template <typename T>
  void set_default_input_value(NodeId for_node, SocketId at_socket, T to) {
    nodes_[for_node]->input_socket<T>(at_socket)->set_default_value(to);
  };

  void propagate_values(NodeId for_node) const {
    // TODO: Currently this function only works if
    // the source and destination sockets are of the same
    // type. It would be a good idea to make this work also
    // if they have types A and B such that A can be casted
    // into B.
    auto neighbors = node(for_node)->get_neighbors();

    std::ranges::for_each(neighbors, [this, for_node](const auto &link) {
      auto [src_socket, dst_node, dst_socket] = link;

      auto source_node = node(for_node);
      auto dest_node = node(dst_node);
      auto output_socket = source_node->get_untyped_output_socket(src_socket);
      auto input_socket = dest_node->get_untyped_input_socket(dst_socket);

      auto src_value = output_socket->get_untyped_current_value();
      input_socket->set_current_value(src_value);
    });
  };
};
}; // namespace qgraph
