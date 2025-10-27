
#include "QGraph/qgraph.hh"
#include "QGraph/qevaluator.hh"
int main() {

  qgraph::Graph g;
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();

  g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                 qgraph::MathNode::Socket::RHS);
  g.connect<int>(1, qgraph::MathNode::Socket::RESULT, 2,
                 qgraph::MathNode::Socket::RHS);

  qgraph::Evaluator eval(g);

  eval.evaluate();
  //
  // auto order = eval.get_execution_order();
}
