#pragma once

#include <QGraph/qsocket.hh>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace qgraph {
class Node {
private:
  std::unordered_map<std::string, std::uint16_t> in_sockets_labels_;
  std::vector<std::shared_ptr<qgraph::Socket>> in_sockets_;
  std::unordered_map<std::string, std::uint16_t> out_sockets_labels_;
  std::vector<std::shared_ptr<qgraph::Socket>> out_sockets_;

public:
  Node() {};

  template <typename T>
  builder::InSocketBuilder<T> add_input_socket(const std::string &label) {
    if (label.empty()) {
      throw std::invalid_argument("Socket label cannot be empty");
    }

    if (in_sockets_labels_.size() >= std::numeric_limits<uint16_t>::max()) {
      throw std::runtime_error("Maximum number of input sockets reached");
    }

    if (!in_sockets_labels_.contains(label)) {
      auto new_socket = std::make_shared<qgraph::InSocket<T>>(label);
      this->in_sockets_.push_back(new_socket);
      new_socket->id = this->in_sockets_.size() - 1;
      in_sockets_labels_.insert({label, new_socket->id});
      return builder::InSocketBuilder<T>(new_socket);
    } else {
      throw std::runtime_error("Input socket with name <" + label +
                               "> already exists");
    }
  };

  template <typename T>
  builder::OutSocketBuilder<T> add_output_socket(const std::string &label) {
    if (label.empty()) {
      throw std::invalid_argument("Socket label cannot be empty");
    }

    if (out_sockets_labels_.size() >= std::numeric_limits<uint16_t>::max()) {
      throw std::runtime_error("Maximum number of output sockets reached");
    }

    if (!out_sockets_labels_.contains(label)) {
      auto new_socket = std::make_shared<qgraph::OutSocket<T>>(label);
      this->out_sockets_.push_back(new_socket);
      new_socket->id = this->out_sockets_.size() - 1;
      out_sockets_labels_.insert({label, new_socket->id});
      return builder::OutSocketBuilder<T>(new_socket);
    } else {
      throw std::runtime_error("Input socket with name <" + label +
                               "> already exists");
    }
  };

  template <typename T>
  std::optional<std::shared_ptr<qgraph::InSocket<T>>>
  get_input_socket(const std::string &label) {
    if (auto id = in_sockets_labels_.find(label);
        id != in_sockets_labels_.end()) {
      auto base_prt = this->in_sockets_[id->second];
      return std::dynamic_pointer_cast<qgraph::InSocket<T>>(base_prt);
    }

    return std::nullopt;
  };

  template <typename T>
  std::optional<std::shared_ptr<qgraph::OutSocket<T>>>
  get_output_socket(const std::string &label) {
    if (auto id = out_sockets_labels_.find(label);
        id != out_sockets_labels_.end()) {
      auto base_ptr = this->out_sockets_[id->second];
      return std::dynamic_pointer_cast<qgraph::OutSocket<T>>(base_ptr);
    }

    return std::nullopt;
  };
};
} // namespace qgraph
