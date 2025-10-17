#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <any>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Validate env") { REQUIRE(true); }

TEST_CASE("Validate default values") {
  InSocket<int> a(10);
  REQUIRE(a.get_default_value() == 10);
}

TEST_CASE("Test connection") {
  InSocket<bool> a(true);

  OutSocket<bool> b(false);
}

TEST_CASE("Get values from node") {
  InSocket<int> a(20);
  OutSocket<bool> b(false);

  Node n;

  n.add_input_socket<int>(a);
  n.add_output_socket<bool>(b);

  // TODO: simplify this!!
  InSocket<std::any> temp = n.get_in_socket(0);
  int cast = std::any_cast<int>(temp.get_default_value());

  REQUIRE(cast == 20);

  OutSocket<std::any> temp1 = n.get_out_socket(0);
  bool cast1 = std::any_cast<bool>(temp1.get_default_value());

  REQUIRE(cast1 == false);
}
