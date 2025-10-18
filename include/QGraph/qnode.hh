#pragma once

#include <QGraph/qsocket.hh>
#include <any>
#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace qgraph {
class Node {
private:
  // std::vector<qgraph::Socket> in_sockets;

public:
  std::unordered_map<std::string, std::uint16_t> in_sockets_labels;
  std::vector<std::unique_ptr<qgraph::Socket>> in_sockets;
  Node() {};

  template <typename T>
  builder::InSocketBuilder<T> add_input_socket(std::string label) {
    if (!in_sockets_labels.contains(label)) {
      auto new_socket = std::make_unique<qgraph::InSocket<T>>(label);

      InSocket<T> *ptr = new_socket.get();

      this->in_sockets.push_back(std::move(new_socket));

      ptr->id = this->in_sockets.size() - 1;
      in_sockets_labels.insert({label, ptr->id});

      return builder::InSocketBuilder<T>(ptr);
    } else {
      throw std::runtime_error("Input socket with name ... already exists");
    }
  };

  template <typename T>
  qgraph::InSocket<T> *get_input_socket(std::string label) {
    // Retrieve index.
    // TODO: Make checks!
    uint16_t id = in_sockets_labels.at(label);

    // Retrieve socket pointer.
    qgraph::Socket *base_prt = this->in_sockets[id].get();

    // Casting to T.
    // TODO: Ensure this casting is posible.
    qgraph::InSocket<T> *derived_ptr =
        dynamic_cast<qgraph::InSocket<T> *>(base_prt);

    return derived_ptr;
  };
};
} // namespace qgraph

class Node {

public:
  std::vector<InSocket<std::any>> in_sockets;
  std::vector<OutSocket<std::any>> out_sockets;

  Node() {};

  template <typename F> void add_input_socket(InSocket<F> in_socket) {
    this->in_sockets.push_back(in_socket.type_erase());
  }

  template <typename F> void add_output_socket(OutSocket<F> out_socket) {
    this->out_sockets.push_back(out_socket.type_erase());
  }

  InSocket<std::any> get_in_socket(std::uint16_t id) {
    if (id < this->in_sockets.size()) {
      return in_sockets[id];
    } else {
      throw std::runtime_error("Out of bounds input socket");
    }
  };

  template <typename T> InSocket<T> get_input_socket(std::uint16_t id) {
    auto type_erased = in_sockets[id];
  };

  OutSocket<std::any> get_out_socket(std::uint16_t id) {
    if (id < this->out_sockets.size()) {
      return out_sockets[id];
    } else {
      throw std::runtime_error("Out of bounds input socket");
    }
  };
};
