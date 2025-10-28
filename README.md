# QGraph

<!--toc:start-->

- [QGraph](#qgraph)
  - [Installation](#installation)
  - [Examples](#examples) - [Creating custom nodes](#creating-custom-nodes) - [Using custom socket types](#using-custom-socket-types)
  <!--toc:end-->

QGraph is a library for building and evaluating computational graphs. It offers the
capacity to create nodes with sockets holding custom types and functions that change
the state of the node whenever called.

## Installation

> Working on this...

## Examples

Here we create a custom nodes, define their behaviour, add them to a graph, connect
them, evaluate the graph and retrieve the result of this computation.

### Creating custom nodes

Every custom node should be derived from the `qgraph::Node` class and override the
`execute` method (unless the node performs no computations). To allow for more
comfortable socket access it is recommended to add an enum with input and
output socket names.

We will create two nodes, a node that sums its inputs and a node holding a constant
value.

```cpp
#include "QGraph/qnode.hh"

class ConstantNode: public qgraph::Node {
public:

  enum Socket {
    Output,
  };

  ConstantNode(const int value) {
    add_output_socket<int>("Output").with_default_value(value);
  };
};
};
```

and

```cpp
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
    auto a = get_input_socket<int>(Socket::LHS).value()->get_current_value();
    auto b = get_input_socket<int>(Socket::RHS).value()->get_current_value();
    switch (this->operation) {

    case SUM:
      get_output_socket<int>(Socket::RESULT).value()->set_current_value(a + b);
      break;
    case SUB:
      get_output_socket<int>(Socket::RESULT).value()->set_current_value(a - b);
      break;
    case MUL:
      get_output_socket<int>(Socket::RESULT).value()->set_current_value(a * b);
      break;
    }
  };
};
```

then you can use this node inside a graph as follows:

```cpp
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
  auto res = g.get_current_output_value<int>(3, MathNode::RESULT).value();

  std::cout << "(100 + 100) * 100 = " << res << "\n";

  return 0;
};

```

### Using custom socket types
