add_executable(qgraph qgraph.cc)
target_link_libraries(
qgraph PRIVATE qgraph::libqgraph
)

include(GNUInstallDirs)
install(
  TARGETS qgraph
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
