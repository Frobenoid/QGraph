#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <string>

namespace qgraph {

class Socket {
public:
  virtual ~Socket() = default;
};

template <typename T> class InSocket : public Socket {
private:
  T default_value_;
  T current_value_;

public:
  // Index in parent node input sockets.
  uint16_t id;
  std::optional<uint16_t> connected_to;
  std::string label;

  InSocket(std::string label) : label(label) {};

  T get_current_value() const { return this->current_value_; };
  T get_default_value() const { return this->default_value_; };

  void set_current_value(T to) { this->current_value_ = to; };
  void set_default_value(T to) { this->default_value_ = to; };

  void connect(uint16_t to_socket) { this->connected_to = to_socket; }
  void disconnect() { this->connected_to.reset(); }
};

template <typename T> class OutSocket : public Socket {
private:
  T default_value_;
  T current_value_;

public:
  // Index inside parent node output sockets.
  uint16_t id;
  std::set<uint16_t> connected_to;
  std::string label;

  OutSocket(std::string label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; }

  void set_current_value(T to) { this->current_value_ = to; };
  void set_default_value(T to) { this->default_value_ = to; };

  void connect(uint16_t to_socket) { this->connected_to.insert(to_socket); };
  void disconnect(uint16_t from) { this->connected_to.erase(from); };
};

namespace builder {

template <typename T> class InSocketBuilder {
private:
  std::shared_ptr<qgraph::InSocket<T>> socket_;

public:
  InSocketBuilder(std::shared_ptr<qgraph::InSocket<T>> socket)
      : socket_(socket) {};

  InSocketBuilder &with_default_value(T default_value) {
    socket_->set_default_value(default_value);
    socket_->set_current_value(default_value);
    return *this;
  };
};

template <typename T> class OutSocketBuilder {
private:
  std::shared_ptr<qgraph::OutSocket<T>> socket_;

public:
  OutSocketBuilder(std::shared_ptr<qgraph::OutSocket<T>> socket)
      : socket_(socket) {};

  OutSocketBuilder &with_default_value(T default_value) {
    socket_->set_default_value(default_value);
    socket_->set_current_value(default_value);
    return *this;
  }
};

} // namespace builder
} // namespace qgraph
