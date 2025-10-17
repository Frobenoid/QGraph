#pragma once

#include <any>
#include <cstdint>
#include <optional>
#include <set>

template <typename T> class InSocket {
private:
  T default_value = current_value;
  T current_value;

public:
  // Index inside parent node.
  uint16_t id;
  std::optional<uint16_t> connected_to;

  InSocket(T default_value) : default_value(default_value) {};

  ~InSocket() {};

  T get_current_value() const { return current_value; };
  T get_default_value() const { return default_value; }

  void set_current_value(T to) { this->current_value = to; };
  void set_default_value(T to) { this->default_value = to; };

  void connect(uint16_t to_socket) { this->connected_to = to_socket; }
  void disconnect() { this->connected_to.reset(); }

  InSocket<std::any> type_erase() {
    // TODO: Cast to any all T dependent props.
    std::any default_value = this->default_value;
    InSocket<std::any> dest(default_value);

    return dest;
  };
};

template <typename T> class OutSocket {
private:
  T default_value;
  T current_value;

public:
  // Index inside parent tree.
  uint16_t id;
  std::set<uint16_t> connected_to = {};

  OutSocket(T default_value) : default_value(default_value) {};

  ~OutSocket() {};

  T get_current_value() const { return current_value; };
  T get_default_value() const { return default_value; }

  void set_current_value(T to) { this->current_value = to; };
  void set_default_value(T to) { this->default_value = to; };

  void connect(uint16_t to_socket) { this->connected_to.insert(to_socket); }
  void disconnect(uint16_t from_socket) {
    this->connected_to.erase(from_socket);
  }
  void disconnect_all() { this->connected_to.clear(); }

  OutSocket<std::any> type_erase() {
    // TODO: Cast to any all T dependent props.
    std::any default_value = this->default_value;
    OutSocket<std::any> dest(default_value);

    return dest;
  };
};

// TODO: Write InSocket<T> -> InSocket<std::any>.
