#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
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

  InSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; };

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };

  void connect(const uint16_t to_socket) { connected_to = to_socket; }
  void disconnect() { connected_to.reset(); }
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

  OutSocket(const std::string &label) : label(label) {};

  T get_current_value() const { return current_value_; };
  T get_default_value() const { return default_value_; }

  void set_current_value(const T to) { current_value_ = to; };
  void set_default_value(const T to) { default_value_ = to; };

  void connect(const uint16_t to_socket) { connected_to.insert(to_socket); };
  void disconnect(const uint16_t from) { connected_to.erase(from); };
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
      throw std::runtime_error("Input socket reference is expired");
    }
  }
};

} // namespace builder
} // namespace qgraph
