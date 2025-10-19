#pragma once

#include <QGraph/qsocket.hh>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace qgraph {
class Node {
private:
  std::unordered_map<std::string, std::uint16_t> in_sockets_labels;
  std::vector<std::unique_ptr<qgraph::Socket>> in_sockets;
  std::unordered_map<std::string, std::uint16_t> out_sockets_labels;
  std::vector<std::unique_ptr<qgraph::Socket>> out_sockets;

public:
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
  builder::OutSocketBuilder<T> add_output_socket(std::string label) {
    if (!out_sockets_labels.contains(label)) {
      auto new_socket = std::make_unique<qgraph::OutSocket<T>>(label);
      OutSocket<T> *ptr = new_socket.get();
      this->out_sockets.push_back(std::move(new_socket));
      ptr->id = this->out_sockets.size() - 1;
      out_sockets_labels.insert({label, ptr->id});

      return builder::OutSocketBuilder<T>(ptr);
    } else {
      throw std::runtime_error("Outsocket with name ... already exists");
    }
  };

  // TODO: Use smart pointer
  template <typename T>
  std::optional<qgraph::InSocket<T> *> get_input_socket(std::string label) {
    if (auto id = in_sockets_labels.find(label);
        id != in_sockets_labels.end()) {

      qgraph::Socket *base_prt = this->in_sockets[id->second].get();

      return dynamic_cast<qgraph::InSocket<T> *>(base_prt);
    }

    return std::nullopt;
  };

  // TODO: Use smart pointer
  template <typename T>
  std::optional<qgraph::OutSocket<T> *> get_output_socket(std::string label) {
    if (auto id = out_sockets_labels.find(label);
        id != out_sockets_labels.end()) {
      qgraph::Socket *base_ptr = this->out_sockets[id->second].get();
      return dynamic_cast<qgraph::OutSocket<T> *>(base_ptr);
    }

    return std::nullopt;
  };
};
} // namespace qgraph
