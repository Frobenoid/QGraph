#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

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
