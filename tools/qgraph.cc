#include "QGraph/qgraph.hh"
#include "QGraph/qevaluator.hh"
#include "QGraph/qnode.hh"
#include <cassert>

class ConstantNode : public qgraph::Node {
public:
  enum Socket {
    Output,
  };

  ConstantNode(const int value) {
    add_output_socket<int>("Output").with_default_value(value);
  };
};

class MathNode : public qgraph::Node {
public:
  enum Socket {
    // Input sockets
    LHS = 0,
    RHS = 1,
    // Output sockets
    RESULT = 0
  };

  enum Operation { SUM, SUB, MUL };

  Operation operation;

  MathNode(Operation operation = SUM) {
    this->operation = operation;
    add_input_socket<int>("A").with_default_value(1);
    add_input_socket<int>("B").with_default_value(1);
    add_output_socket<int>("C").with_default_value(0);
  };

  void execute() override {
    auto a = input_socket<int>(Socket::LHS)->current_value();
    auto b = input_socket<int>(Socket::RHS)->current_value();
    switch (this->operation) {

    case SUM:
      output_socket<int>(Socket::RESULT)->set_current_value(a + b);
      break;
    case SUB:
      output_socket<int>(Socket::RESULT)->set_current_value(a - b);
      break;
    case MUL:
      output_socket<int>(Socket::RESULT)->set_current_value(a * b);
      break;
    }
  };
};

int main() {
  qgraph::Graph g;
  qgraph::Evaluator eval(g);

  g.add_node<ConstantNode>(100);
  g.add_node<ConstantNode>(100);
  g.add_node<MathNode>();
  g.add_node<MathNode>(MathNode::Operation::MUL);

  // Connecting the nodes.
  g.connect<int>(0, ConstantNode::Output, 2, MathNode::LHS);
  g.connect<int>(1, ConstantNode::Output, 2, MathNode::RHS);
  g.connect<int>(2, MathNode::RESULT, 3, MathNode::LHS);
  g.connect<int>(0, ConstantNode::Output, 3, MathNode::RHS);

  // Evaluating the graph.
  eval.evaluate();

  // Getting the result.
  auto res = g.current_output_value<int>(3, MathNode::RESULT);

  std::cout << "(100 + 100) * 100 = " << res << "\n";

  return 0;
};
