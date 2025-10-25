#include "QGraph/qevaluator.hh"
#include "QGraph/qgraph.hh"
#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <string_view>
#include <vector>

TEST_CASE("Socket builder", "[socket]") {
  qgraph::Node n;
  n.add_input_socket<int>("Input").with_default_value(10);
  n.add_output_socket<bool>("Condition").with_default_value(false);

  auto a = n.get_input_socket<int>("Input").value();
  auto b = n.get_output_socket<bool>("Condition").value();

  REQUIRE(a->get_default_value() == 10);
  REQUIRE(a->get_current_value() == 10);
  REQUIRE(a->label == std::string_view("Input"));

  REQUIRE(b->get_default_value() == false);
  REQUIRE(b->get_current_value() == false);
  REQUIRE(b->label == std::string_view("Condition"));
}

TEST_CASE("Change current value", "[socket]") {
  qgraph::Node n;
  n.add_input_socket<float>("A").with_default_value(10.0);
  n.get_input_socket<float>("A").value()->set_current_value(11.5);

  REQUIRE(n.get_input_socket<float>("A").value()->get_current_value() == 11.5);
  REQUIRE(n.get_input_socket<float>("A").value()->get_default_value() == 10.0);
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

TEST_CASE("Socket connection", "[socket, connection]") {
  qgraph::InSocket<int> a("a");

  a.connect(0, 0);

  REQUIRE(a.connected_to->destination_node == 0);
  REQUIRE(a.connected_to->destination_socket == 0);

  qgraph::OutSocket<int> b("b");

  b.connect(0, 0);

  REQUIRE(b.connected_to.contains({0, 0, 0}));

  b.disconnect(0, 0);

  REQUIRE_FALSE(b.connected_to.contains({0, 0, 0}));
}

TEST_CASE("Tree construction", "[tree, node]") {
  qgraph::Graph g;

  SECTION("Node addition and retrieval") {
    g.add_node<qgraph::IncrNode>();

    REQUIRE(g.nodes.size() == 1);

    REQUIRE(g.get_node(0)
                ->get_output_socket<int>("Value")
                .value()
                ->get_default_value() == 0);
  }

  SECTION("Node deletion") {
    g.add_node<qgraph::IncrNode>();

    g.delete_node(0);

    REQUIRE(g.nodes.size() == 0);
  }

  SECTION("Node connection") {
    g.add_node<qgraph::MathNode>();
    g.add_node<qgraph::MathNode>();

    g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                   qgraph::MathNode::Socket::LHS);

    auto source =
        g.get_node(0)->get_output_socket<int>("C").value()->connected_to;
    auto dest = g.get_node(1)->get_input_socket<int>("A").value()->connected_to;

    REQUIRE(source.size() == 1);
    REQUIRE(dest.has_value());
  }
}

TEST_CASE("Tree evaluation", "[tree, evaluation]") {
  // (1 + (1 + (1+ 1)))

  qgraph::Graph g;
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();
  g.add_node<qgraph::MathNode>();

  g.connect<int>(0, qgraph::MathNode::Socket::RESULT, 1,
                 qgraph::MathNode::Socket::RHS);
  g.connect<int>(1, qgraph::MathNode::Socket::RESULT, 2,
                 qgraph::MathNode::Socket::RHS);

  REQUIRE(g.nodes.size() == 3);

  qgraph::Evaluator eval(g);
  eval.evaluate();

  auto order = eval.get_execution_order();

  REQUIRE(order[0] == 0);
  REQUIRE(order[1] == 1);
  REQUIRE(order[2] == 2);

  REQUIRE(g.get_current_output_value<int>(2, qgraph::MathNode::Socket::RESULT)
              .value() == 4);
}
