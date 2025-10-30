#pragma once
#include <QGraph/qtypes.hh>
#include <tuple>

namespace qgraph {
struct Link {
  SocketId source_socket;
  NodeId destination_node;
  SocketId destination_socket;

  bool operator<(const Link &rhs) const {
    return std::tie(source_socket, destination_node, destination_socket) <
           std::tie(rhs.source_socket, rhs.destination_node,
                    rhs.destination_socket);
  }
};
} // namespace qgraph
