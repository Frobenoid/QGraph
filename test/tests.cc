#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Validate env") { REQUIRE(true); }

TEST_CASE("Validate default values") {
  InSocket<int> a(10);
  REQUIRE(a.get_default_value() == 10);
}

TEST_CASE("Get values from node") {
  InSocket<int> a(20);
  OutSocket<bool> b(false);

  Node n;

  n.add_input_socket<int>(a);
  n.add_output_socket<bool>(b);

  auto a_cast = n.get_in_socket(0).cast_to<int>();
  auto b_cast = n.get_out_socket(0).cast_to<bool>();

  REQUIRE(a_cast.get_default_value() == 20);

  REQUIRE(b_cast.get_default_value() == false);
}

TEST_CASE("Set current, get current") {
  InSocket<int> a(20);
  a.set_current_value(100);

  Node n;

  n.add_input_socket(a);

  REQUIRE(n.get_in_socket(0).cast_to<int>().get_current_value() == 100);
}
