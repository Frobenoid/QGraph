#pragma once

#include <QGraph/qlink.hh>
#include <QGraph/qtypes.hh>
#include <any>
#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>

namespace qgraph {

class Socket {
public:
  AssociatedType type;

  SocketId id;
  virtual ~Socket() = default;
  virtual std::set<Link> get_neighbors() const { return {}; };
  virtual void set_current_value(const std::any to) {};
  virtual std::any get_untyped_current_value() const { return std::any(0); };
};

template <typename T> class InSocket : public Socket {
private:
  T default_value_;
  T current_value_;

public:
  // Index in parent node input sockets.
  SocketId id;
  std::optional<Link> connected_to;
  std::string label;

  InSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; };

  void set_current_value(const std::any to) override {
    current_value_ = std::any_cast<T>(to);
  };

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };
  void set_default_value(const std::any to) {
    default_value_ = std::any_cast<T>(to);
  };

  void connect(const qgraph::NodeId to_node, const qgraph::SocketId at_socket) {
    connected_to = Link{id, to_node, at_socket};
  };

  void disconnect() { connected_to.reset(); };
};

template <typename T> class OutSocket : public Socket {
private:
  T default_value_;
  T current_value_;

public:
  // Index inside parent node output sockets.
  qgraph::SocketId id;
  // Set containing all input sockets this is connected to.
  std::set<Link> connected_to;
  // Label of the node.
  std::string label;

  OutSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; }

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };

  void connect(const qgraph::NodeId to_node, const qgraph::SocketId at_socket) {
    connected_to.emplace(id, to_node, at_socket);
  };

  void disconnect(const uint16_t to_node, const uint16_t at_socket) {
    connected_to.erase({id, to_node, at_socket});
  };

  std::set<Link> get_neighbors() const override { return this->connected_to; }

  std::any get_untyped_current_value() const override {
    return std::any(current_value_);
  };

  std::any get_untyped_default_value() const {
    return std::any(default_value_);
  };
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
