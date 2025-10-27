#pragma once
#include <QGraph/qtypes.hh>

namespace qgraph {
class Link {
public:
  SocketId source_socket;
  NodeId destination_node;
  SocketId destination_socket;

  bool operator<(const Link &rhs) const {
    return source_socket < rhs.source_socket &&
           destination_node < rhs.destination_node &&
           destination_socket < rhs.destination_socket;
  }

  bool operator==(const Link &rhs) const {
    return source_socket == rhs.source_socket &&
           destination_node == rhs.destination_node &&
           destination_socket == rhs.destination_socket;
  }
};
} // namespace qgraph
