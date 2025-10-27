# QGraph

<!--toc:start-->

- [QGraph](#qgraph)
- [Installation](#installation)
- [Examples](#examples)
  - [Creating a node](#creating-a-node)
  <!--toc:end-->

QGraph is a library for building and evaluating computational graphs. It offers the
capacity to create nodes with sockets holding custom types and functions that change
the state of the node whenever called.

## Installation

> Working on this...

## Examples

Here we create a custom nodes, define their behaviour, add them to a graph, connect
them, evaluate the graph and retrieve the result of this computation.

## Creating a node

Every custom node should be derived from the `qgraph::Node` class and override the
`execute` method. To allow for more comfortable socket access it is recommended to
add an enum with input and output socket names.

We will create two nodes, a node that sums its inputs and a node holding a constant
value.

```cpp
#include "QGraph/qnode.hh"

class ConstantNode: public qgraph::Node {
public:

  enum Socket {
    Output,
  };

  ConstantNode() {
    add_output_socket<int>("Output").with_default_value(0);
  };

  // This node performs no computations.
  void execute() override {
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

  MathNode() {
    add_input_socket<int>("LHS").with_default_value(1);
    add_input_socket<int>("RHS").with_default_value(1);
    add_output_socket<int>("RESULT").with_default_value(0);
  };

  void execute() override {
    auto a = get_input_socket<int>("A").value()->get_current_value();
    auto b = get_input_socket<int>("B").value()->get_current_value();
    get_output_socket<int>("C").value()->set_current_value(a + b);
  };
};
```

then you can use this node inside a graph as follows:

```cpp
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

```
