#pragma once

#include <QGraph/qgraph.hh>
#include <iostream>
#include <vector>

namespace qgraph {
class Evaluator {
private:
  enum Color { WHITE = 0, GRAY = 1, BLACK = 2 };

  Graph &graph_;
  std::vector<NodeId> execution_order_;
  std::set<NodeId> visited_;
  bool is_valid_ = true;
  std::unordered_map<NodeId, Color> colors_;

  /// This function checks if there is
  /// a directed cycle in the current graph
  /// and computes the evaluation order.
  ///
  /// This is done by topological sorting the
  /// graph. If at some point the sorting detects
  /// a directed cycle, this function throws.
  void verify_integrity() { dfs(); };

  // Recursive depth firt search.
  void dfs() {
    for (int i = 0; i < graph_.get_number_of_nodes(); i++) {
      colors_[i] = WHITE;
    }

    for (int i = 0; i < graph_.get_number_of_nodes(); i++) {
      if (colors_[i] == WHITE) {
        dfs_visit(i);
      }
    }
  };

  void dfs_visit(NodeId node) {
    colors_[node] = GRAY;

    visited_.insert(node);

    for (auto link : graph_.get_node(node)->get_neighbors()) {
      if (colors_[link.destination_node] == WHITE) {
        dfs_visit(link.destination_node);
      } else if (colors_[link.destination_node] == GRAY) {
        is_valid_ = false;
        std::cerr
            << "WARN: Provided graph is not valid. Contains a directed cycle\n";
        return;
      }
    }

    // Finished exploring node.
    execution_order_.push_back(node);
    colors_[node] = BLACK;
  };

public:
  Evaluator(Graph &graph) : graph_(graph) {};

  auto get_execution_order() { return execution_order_; };

  void evaluate() {

    verify_integrity();

    if (is_valid_) {
      for (int i = execution_order_.size() - 1; 0 <= i; i--) {
        // Execute node.
        graph_.nodes[execution_order_[i]]->execute();
        // Propagate values.
        graph_.propagate_values(execution_order_[i]);
      }
    } else {
      return;
    }
  };

  bool is_valid() { return is_valid_; }
};
} // namespace qgraph
