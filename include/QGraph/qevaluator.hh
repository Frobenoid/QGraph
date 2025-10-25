#pragma once

#include <QGraph/qgraph.hh>
#include <cstdint>
#include <deque>

namespace qgraph {
class Evaluator {
private:
  Graph &graph_;
  // FIX: Use a vec for contiguous memory access.
  std::deque<NodeId> execution_order_;
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
    uint16_t time = 0;
    // TODO: Is it cheaper to use indices?
    // Profile this and make a decision.
    for (auto node : graph_.nodes) {
      if (!visited_.contains(node->id)) {
        dfs_visit(node->id, time);
      }
    }
  };

  void dfs_visit(NodeId node, uint16_t &time) {
    time += 1;

    visited_.insert(node);

    for (auto link : graph_.get_node(node)->get_neighbors()) {
      if (!visited_.contains(node)) {
        dfs_visit(link.destination_node, time);
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
    for (auto node_id : execution_order_) {
      // Execute node.
      graph_.nodes[node_id]->execute();
      // Propagate values.
      graph_.propagate_values(node_id);
    }
  };
};
} // namespace qgraph
