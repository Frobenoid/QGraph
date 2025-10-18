#include <QGraph/qnode.hh>
#include <QGraph/qsocket.hh>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Socket builder") {
  qgraph::Node n;
  n.add_input_socket<int>("Input").with_default_value(10);

  auto tmp = n.get_input_socket<int>("Input");

  REQUIRE(tmp->get_default_value() == 10);
  REQUIRE(tmp->get_current_value() == 10);
  REQUIRE(tmp->label == "Input");
}
