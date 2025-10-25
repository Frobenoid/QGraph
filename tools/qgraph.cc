
#include "QGraph/qgraph.hh"
#include "QGraph/qevaluator.hh"
#include <iostream>
int main() {

  qgraph::Graph g;
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();

  g.connect<int>(0, qgraph::MathNode::Socket::C, 1,
                 qgraph::MathNode::Socket::B);

  qgraph::Evaluator eval(g);

  eval.evaluate();

  auto order = eval.get_execution_order();
}
