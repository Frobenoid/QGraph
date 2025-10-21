#pragma once
#include "QGraph/qnode.hh"
#include <type_traits>

namespace qgraph {
template <typename T>
concept ConceptObject = std::is_base_of<qgraph::Node, T>::value;

class Graph {
public:
  template <ConceptObject T>
  void add_node() {

  };
};
}; // namespace qgraph
