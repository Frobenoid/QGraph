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
  std::unordered_map<std::string, std::uint16_t> out_sockets_labels_;

public:
  Node() {};
  /// Index in parent graph.
  NodeId id;

  std::vector<std::shared_ptr<qgraph::Socket>> in_sockets_;
  std::vector<std::shared_ptr<qgraph::Socket>> out_sockets_;

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
      return std::static_pointer_cast<qgraph::InSocket<T>>(base_prt);
    }

    return std::nullopt;
  };

  template <typename T>
  std::optional<std::shared_ptr<InSocket<T>>>
  get_input_socket(const SocketId id) {
    if (id < in_sockets_.size()) {
      return std::static_pointer_cast<InSocket<T>>(in_sockets_[id]);
    };
    return std::nullopt;
  };

  template <typename T>
  std::optional<std::shared_ptr<qgraph::OutSocket<T>>>
  get_output_socket(const std::string &label) {
    if (auto id = out_sockets_labels_.find(label);
        id != out_sockets_labels_.end()) {
      auto base_ptr = this->out_sockets_[id->second];
      return std::static_pointer_cast<qgraph::OutSocket<T>>(base_ptr);
    }

    return std::nullopt;
  };

  template <typename T>
  std::optional<std::shared_ptr<OutSocket<T>>>
  get_output_socket(const SocketId id) {
    if (id < out_sockets_.size()) {
      return std::static_pointer_cast<OutSocket<T>>(out_sockets_[id]);
    };
    return std::nullopt;
  };

  auto get_neighbors() const {
    // TODO: Improve this.
    std::vector<Link> v;
    for (auto x : out_sockets_) {
      for (auto y : x->get_neighbors()) {
        v.push_back(y);
      }
    }
    return v;
  };

  AssociatedType get_type_of_output_socket(SocketId socket_id) {
    if (socket_id < this->out_sockets_.size()) {
      return out_sockets_[socket_id]->type;
    } else {
      throw std::runtime_error(&"NOPE: "[socket_id]);
    }
  };

  virtual void execute() {};
};

class MathNode : public Node {
public:
  enum Socket {
    // Input sockets
    LHS = 0,
    RHS = 1,
    // Output sockets
    RESULT = 0
  };

  MathNode() {
    add_input_socket<int>("A").with_default_value(1);
    add_input_socket<int>("B").with_default_value(1);
    add_output_socket<int>("C").with_default_value(0);
  };

  void execute() override {
    auto a = get_input_socket<int>("A").value()->get_current_value();
    auto b = get_input_socket<int>("B").value()->get_current_value();
    get_output_socket<int>("C").value()->set_current_value(a + b);
  };
};

class IncrNode : public Node {
public:
  IncrNode() {
    add_input_socket<int>("Value").with_default_value(10);
    add_input_socket<bool>("Condition").with_default_value(true);
    add_output_socket<int>("Value").with_default_value(0);
  };

  void execute() override {
    bool condition =
        get_input_socket<bool>("Condition").value()->get_current_value();

    int value = get_input_socket<int>("Value").value()->get_current_value();
    auto out = get_output_socket<int>("Value").value();

    if (condition) {
      out->set_current_value(value + 1);
    }
  };
};

class ConstantNode : public Node {
public:
  enum Socket { Value = 0 };

  ConstantNode() { add_output_socket<int>("Output").with_default_value(0); }
  void execute() override {};
};
} // namespace qgraph
