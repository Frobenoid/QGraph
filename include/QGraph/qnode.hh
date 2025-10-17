#pragma once

#include <QGraph/qsocket.hh>
#include <any>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
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
      std::runtime_error("Out of bounds input socket");
    }
  };

  template <typename T> InSocket<T> get_input_socket(std::uint16_t id) {
    auto type_erased = in_sockets[id];
  };

  OutSocket<std::any> get_out_socket(std::uint16_t id) {
    if (id < this->out_sockets.size()) {
      return out_sockets[id];
    } else {
      std::runtime_error("Out of bounds input socket");
    }
  };
};

// template <typename T> class Node {
// public:
//   Node() {};
//   std::vector<InSocket<std::any>> input_sockets;
//   std::vector<OutSocket<std::any>> out_sockets;
//
//   InSocket<std::any> get_input_socket(uint16_t id) {
//     if (id < this->input_sockets.size()) {
//       return input_sockets[id];
//     } else {
//       std::runtime_error("Out of bounds input socket");
//     }
//   };
//
//   template <typename F> InSocket<F> get_typed_in_socket(uint16_t id) {
//     if (id < this->input_sockets.size()) {
//       InSocket<std::any> t = this->input_sockets[id];
//
//       // Ensure the retrieved socket can be casted from
//       // std::any to F.
//       // assert(t.get_default_value().type().name() ==
//       std::string_view("int"));
//
//       // TODO: Implement from...
//       // InSocket<F> cast = InSocket<F>::from(t);
//       //
//       F default_value = std::any_cast<F>(t.get_default_value());
//       InSocket<F> cast = InSocket<F>(default_value);
//
//       return cast;
//     } else {
//       std::runtime_error("Out of bounds input socket");
//     }
//   }
//
//   // template <typename F> InSocket<F> get_input_socket(uint16_t id) {
//   //   if (id < this->input_sockets.size()) {
//   //     return std::any_cast<InSocket<F>>(input_sockets[id]);
//   //   } else {
//   //     std::runtime_error("Out of bounds input socket");
//   //   }
//   // };
//   //
//   // template <typename F> OutSocket<F> get_output_socket(uint16_t id) {
//   //   if (id < this->input_sockets.size()) {
//   //     return (OutSocket<F>)out_sockets[id];
//   //   } else {
//   //     std::runtime_error("Out of bounds input socket");
//   //   }
//   // }
//
//   template <typename F> void add_input_socket(InSocket<F> in_socket) {
//     this->input_sockets.push_back((InSocket<std::any>)in_socket);
//   }
//
//   template <typename F> void add_output_socket(OutSocket<F> out_socket) {
//     this->out_sockets.push_back((OutSocket<std::any>)out_socket);
//   }
// };
