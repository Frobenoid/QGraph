#pragma once

#include <QGraph/qgraph.hh>
#include <vector>

namespace qgraph {
class Evaluator {
private:
  Graph &graph_;
  std::vector<NodeId> execution_order_;
  std::set<NodeId> visited_;

  /// This function checks if there is
  /// a directed cycle in the current graph
  /// and computes the evaluation order.
  ///
  /// This is done by topological sorting the
  /// graph. If at some point the sorting detects
  /// a directed cycle, this function throws.
  void verify_integrity() {
    // 1. Compute finish times for each node.
    // 2. As each vertex is finished, it will
    //    placed in the front of `execution_order`.
    // 3. Return `execution_order`.
    dfs();
  };

  // Recursive depth firt search.
  void dfs() {
    // TODO: Is it cheaper to use indices?
    // Profile this and make a decision.
    for (auto node : graph_.nodes) {
      if (!visited_.contains(node->id)) {
        dfs_visit(node->id);
      }
    }
  };

  void dfs_visit(NodeId node) {
    visited_.insert(node);

    for (auto link : graph_.get_node(node)->get_neighbors()) {
      if (!visited_.contains(link.destination_node)) {
        dfs_visit(link.destination_node);
      }
    }

    // Finished exploring node.
    execution_order_.push_back(node);
  };

public:
  Evaluator(Graph &graph) : graph_(graph) {};

  auto get_execution_order() { return execution_order_; };

  void evaluate() {
    verify_integrity();

    // Execute nodes.
    // LOOP INVARIANT: Inputs are ready to be used.
    for (int i = execution_order_.size() - 1; 0 <= i; i--) {
      // Execute node.
      graph_.nodes[execution_order_[i]]->execute();
      // Propagate values.
      graph_.propagate_values(execution_order_[i]);
    }
  };
};
} // namespace qgraph
