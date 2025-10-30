#include "QGraph/qevaluator.hh"
#include "QGraph/qgraph.hh"
#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <string_view>

TEST_CASE("Socket builder", "[socket]") {
  qgraph::Node n;
  n.add_input_socket<int>("Input").with_default_value(10);
  n.add_output_socket<bool>("Condition").with_default_value(false);

  auto a = n.get_input_socket<int>("Input").value();
  auto b = n.get_output_socket<bool>("Condition").value();

  REQUIRE(a->default_value() == 10);
  REQUIRE(a->current_value() == 10);
  REQUIRE(a->label() == std::string_view("Input"));

  REQUIRE(b->get_default_value() == false);
  REQUIRE(b->get_current_value() == false);
  REQUIRE(b->label == std::string_view("Condition"));
}

TEST_CASE("Change current value", "[socket]") {
  qgraph::Node n;
  n.add_input_socket<float>("A").with_default_value(10.0);
  n.get_input_socket<float>("A").value()->set_current_value(11.5);

  REQUIRE(n.get_input_socket<float>("A").value()->current_value() == 11.5);
  REQUIRE(n.get_input_socket<float>("A").value()->default_value() == 10.0);
}

TEST_CASE("Nodes", "[node]") {
  qgraph::MathNode math;

  math.execute();

  auto t = math.get_output_socket<int>("C").value()->get_current_value();

  REQUIRE(t == 2);

  qgraph::IncrNode incr;

  incr.execute();

  auto f = incr.get_output_socket<int>("Value").value()->get_current_value();

  REQUIRE(f == 11);
}

TEST_CASE("Tree construction", "[graph, node]") {
  qgraph::Graph g;

  SECTION("Node addition and retrieval") {
    g.add_node<qgraph::IncrNode>();

    REQUIRE(g.num_of_nodes() == 1);

    REQUIRE(g.get_node(0)
                ->get_output_socket<int>("Value")
                .value()
                ->get_default_value() == 0);
  }

  SECTION("Node deletion") {
    g.add_node<qgraph::IncrNode>();

    g.delete_node(0);

    REQUIRE(g.num_of_nodes() == 0);
  }

  SECTION("Node connection") {
    g.add_node<qgraph::MathNode>();
    g.add_node<qgraph::MathNode>();

    g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                   qgraph::MathNode::Socket::LHS);

    auto source =
        g.get_node(0)->get_output_socket<int>("C").value()->connected_to;
    auto dest =
        g.get_node(1)->get_input_socket<int>("A").value()->connected_to();

    REQUIRE(source.size() == 1);
    REQUIRE(dest.has_value());
  }
}

TEST_CASE("Tree evaluation", "[graph, evaluation]") {
  // (1 + (1 + (1+ 1)))

  qgraph::Graph g;
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();

  g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                 qgraph::MathNode::Socket::RHS);
  g.connect<int>(1, qgraph::MathNode::Socket::RESULT, 2,
                 qgraph::MathNode::Socket::RHS);

  REQUIRE(g.num_of_nodes() == 3);

  qgraph::Evaluator eval(g);
  eval.evaluate();

  REQUIRE(eval.is_valid());

  auto order = eval.get_execution_order();

  REQUIRE(g.get_current_output_value<int>(2, qgraph::MathNode::Socket::RESULT)
              .value() == 4);
}

TEST_CASE("Evaluation order", "[graph, evaluation]") {
  qgraph::Graph g;

  g.add_node<qgraph::ConstantNode>(); // 0
  g.add_node<qgraph::ConstantNode>(); // 1
  g.add_node<qgraph::ConstantNode>(); // 2
  g.add_node<qgraph::ConstantNode>(); // 3

  g.add_node<qgraph::MathNode>(); // 4
  g.add_node<qgraph::MathNode>(); // 5
  g.add_node<qgraph::MathNode>(); // 6

  g.set_current_output_value<int>(0, qgraph::ConstantNode::Socket::Value, 10);
  g.set_current_output_value<int>(1, qgraph::ConstantNode::Socket::Value, 30);
  g.set_current_output_value<int>(2, qgraph::ConstantNode::Socket::Value, 40);
  g.set_current_output_value<int>(3, qgraph::ConstantNode::Socket::Value, 20);

  g.connect<int>(0, qgraph::ConstantNode::Socket::Value, 5,
                 qgraph::MathNode::Socket::LHS);
  g.connect<int>(3, qgraph::ConstantNode::Socket::Value, 5,
                 qgraph::MathNode::Socket::RHS);

  g.connect<int>(5, qgraph::MathNode::Socket::RESULT, 6,
                 qgraph::MathNode::Socket::LHS);
  g.connect<int>(1, qgraph::ConstantNode::Socket::Value, 6,
                 qgraph::MathNode::Socket::RHS);

  g.connect<int>(6, qgraph::MathNode::Socket::RESULT, 4,
                 qgraph::MathNode::Socket::LHS);
  g.connect<int>(2, qgraph::ConstantNode::Socket::Value, 4,
                 qgraph::MathNode::Socket::RHS);

  qgraph::Evaluator eval(g);

  eval.evaluate();

  REQUIRE(g.get_current_output_value<int>(5, qgraph::MathNode::Socket::RESULT)
              .value() == 30);
  REQUIRE(g.get_current_output_value<int>(6, qgraph::MathNode::Socket::RESULT)
              .value() == 60);
  REQUIRE(g.get_current_output_value<int>(4, qgraph::MathNode::Socket::RESULT)
              .value() == 100);
}

TEST_CASE("Invalid graph", "[graph, validation]") {
  qgraph::Graph g;

  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();

  g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                 qgraph::MathNode::Socket::LHS);

  g.connect<int>(1, qgraph::MathNode::Socket::RESULT, 0,
                 qgraph::MathNode::Socket::LHS);

  qgraph::Evaluator eval(g);

  eval.evaluate();

  REQUIRE_FALSE(eval.is_valid());
}
