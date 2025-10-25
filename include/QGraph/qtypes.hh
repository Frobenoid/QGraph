#pragma once
#include <cstdint>

namespace qgraph {

using NodeId = uint16_t;
using SocketId = uint16_t;

enum AssociatedType {
  INT,
  BOOL,
  STRING,
};

} // namespace qgraph
