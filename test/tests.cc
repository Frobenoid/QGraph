#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

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

TEST_CASE("Multiple ins, multiple outs") {
  InSocket<int> a(10);
  InSocket<std::string> b("Test");
  InSocket<bool> c(true);

  Node n;

  n.add_input_socket(a);
  n.add_input_socket(b);
  n.add_input_socket(c);

  REQUIRE(n.get_in_socket(0).cast_to<int>().get_default_value() == 10);
  REQUIRE(n.get_in_socket(1).cast_to<std::string>().get_default_value() ==
          std::string_view("Test"));
  REQUIRE(n.get_in_socket(2).cast_to<bool>().get_default_value() == true);

  OutSocket<int> a_out(10);
  OutSocket<std::string> b_out("Test");
  OutSocket<bool> c_out(true);

  n.add_output_socket(a_out);
  n.add_output_socket(b_out);
  n.add_output_socket(c_out);

  REQUIRE(n.get_out_socket(0).cast_to<int>().get_default_value() == 10);
  REQUIRE(n.get_out_socket(1).cast_to<std::string>().get_default_value() ==
          std::string_view("Test"));
  REQUIRE(n.get_out_socket(2).cast_to<bool>().get_default_value() == true);
}
