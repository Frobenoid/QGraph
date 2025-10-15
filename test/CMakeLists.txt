add_executable(tests tests.cc)
target_link_libraries(tests PRIVATE qgraph::libqgraph Catch2::Catch2WithMain)
