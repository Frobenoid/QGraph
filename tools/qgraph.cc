#include "QGraph/qgraph.hh"
#include "QGraph/qevaluator.hh"
#include "QGraph/qnode.hh"
#include <cassert>

class ConstantNode : public qgraph::Node {
public:
  enum Socket {
    Output,
  };

  ConstantNode() { add_output_socket<int>("Output").with_default_value(0); };

  // This node performs no computations.
  void execute() override {};
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

  MathNode() {
    add_input_socket<int>("A").with_default_value(1);
    add_input_socket<int>("B").with_default_value(1);
    add_output_socket<int>("C").with_default_value(0);
  };

  void execute() override {
    auto a = get_input_socket<int>(Socket::LHS).value()->get_current_value();
    auto b = get_input_socket<int>(Socket::RHS).value()->get_current_value();
    get_output_socket<int>(Socket::RESULT).value()->set_current_value(a + b);
  };
};

int main() {
  qgraph::Graph g;
  qgraph::Evaluator eval(g);

  g.add_node<ConstantNode>();
  g.add_node<ConstantNode>();
  g.add_node<MathNode>();

  // Set the constant values.
  g.set_current_output_value(0, ConstantNode::Output, 1);
  g.set_current_output_value(1, ConstantNode::Output, 1);

  // Connecting the nodes.
  g.connect<int>(0, ConstantNode::Output, 2, MathNode::LHS);
  g.connect<int>(1, ConstantNode::Output, 2, MathNode::RHS);

  // Evaluating the graph.
  eval.evaluate();

  // Getting the result.
  auto res = g.get_current_output_value<int>(2, MathNode::RESULT).value();

  std::cout << "1 + 1 = " << res << "\n";

  return 0;
};
