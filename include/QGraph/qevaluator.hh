#pragma once

#include <QGraph/qgraph.hh>
#include <algorithm>
#include <iostream>
#include <ranges>
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
    std::ranges::fill(colors_ | std::views::values, WHITE);

    for (int i : std::views::iota(0, graph_.get_number_of_nodes())) {
      if (colors_[i] == WHITE) {
        dfs_visit(i);
      }
    }
  };

  void dfs_visit(NodeId node) {
    colors_[node] = GRAY;

    visited_.insert(node);

    auto neighbors = graph_.get_node(node)->get_neighbors();

    // Check if there are directed cycles.
    if (std::ranges::any_of(neighbors, [this](const auto &link) {
          return colors_[link.destination_node] == GRAY;
        })) {
      is_valid_ = false;
      std::cerr
          << "WARN: Provided graph is not valid. Contains a directed cycle\n";
      return;
    }

    // Visit unexplored nodes.
    std::ranges::for_each(
        neighbors | std::views::filter([this](const auto &link) {
          return colors_[link.destination_node] == WHITE;
        }),
        [this](const auto &link) { dfs_visit(link.destination_node); });

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
      std::ranges::for_each(execution_order_ | std::views::reverse,
                            [this](const auto node) {
                              graph_.execute_node(node);
                              graph_.propagate_values(node);
                            });
    } else {
      return;
    }
  };

  bool is_valid() { return is_valid_; }
};
} // namespace qgraph
