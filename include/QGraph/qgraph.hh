#pragma once
#include "QGraph/qnode.hh"
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
    std::shared_ptr<T> ptr = std::make_shared<T>();
    nodes.push_back(ptr);
  };
};
}; // namespace qgraph
