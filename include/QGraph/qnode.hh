#pragma once

#include <QGraph/qsocket.hh>
#include <any>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

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
