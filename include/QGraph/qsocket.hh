#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

namespace qgraph {

using NodeId = uint16_t;
using SocketId = uint16_t;

class Socket {
public:
  virtual ~Socket() = default;
  virtual std::set<std::pair<NodeId, SocketId>> get_neighbors() const {
    return {};
  };
};

template <typename T> class InSocket : public Socket {
private:
  // T default_value_;
  // T current_value_;

public:
  T default_value_;
  T current_value_;
  // Index in parent node input sockets.
  qgraph::SocketId id;
  std::optional<std::pair<qgraph::NodeId, qgraph::SocketId>> connected_to;
  std::string label;

  InSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; };

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };

  // TODO: Implement validation:
  // 1. Node exists in parent tree?
  // 2. Node contains input socket at index?
  void connect(const qgraph::NodeId to_node, const qgraph::SocketId at_socket) {
    connected_to = std::pair(to_node, at_socket);
  };

  void disconnect() { connected_to.reset(); };
};

template <typename T> class OutSocket : public Socket {
private:
  // T default_value_;
  // T current_value_;

public:
  T default_value_;
  T current_value_;
  // Index inside parent node output sockets.
  qgraph::SocketId id;
  std::set<std::pair<qgraph::NodeId, qgraph::SocketId>> connected_to;
  std::string label;

  OutSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; }

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };

  // TODO: Implement validation
  // 1. Node exists in parent tree?
  // 2. Node contains input socket at index?
  void connect(const qgraph::NodeId to_node, const qgraph::SocketId at_socket) {
    connected_to.insert(std::pair(to_node, at_socket));
  };

  // TODO: Implement validation
  void disconnect(const uint16_t to_node, const uint16_t at_socket) {
    connected_to.erase(std::pair(to_node, at_socket));
  };

  std::set<std::pair<NodeId, SocketId>> get_neighbors() const override {
    return this->connected_to;
  }
};

namespace builder {

template <typename T> class InSocketBuilder {
private:
  std::weak_ptr<qgraph::InSocket<T>> socket_;

public:
  InSocketBuilder(std::shared_ptr<qgraph::InSocket<T>> socket)
      : socket_(socket) {};

  InSocketBuilder &with_default_value(const T default_value) {
    if (auto ptr = socket_.lock()) {
      ptr->set_default_value(default_value);
      ptr->set_current_value(default_value);
      return *this;
    } else {
      throw std::runtime_error("Input socket reference is expired");
    }
  };
};

template <typename T> class OutSocketBuilder {
private:
  std::weak_ptr<qgraph::OutSocket<T>> socket_;

public:
  OutSocketBuilder(std::shared_ptr<qgraph::OutSocket<T>> socket)
      : socket_(socket) {};

  OutSocketBuilder &with_default_value(const T default_value) {
    if (auto ptr = socket_.lock()) {
      ptr->set_default_value(default_value);
      ptr->set_current_value(default_value);
      return *this;
    } else {
      throw std::runtime_error("Ouput socket reference is expired");
    }
  }
};

} // namespace builder
} // namespace qgraph
