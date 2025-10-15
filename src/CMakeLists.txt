add_library(libqgraph libqgraph.cc)
add_library(qgraph::libqgraph ALIAS libqgraph)

set_target_properties(
libqgraph
PROPERTIES OUTPUT_NAME qgraph
)

target_compile_features(
  libqgraph PUBLIC cxx_std_17
)

target_include_directories(libqgraph
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  PRIVATE ${CMAKE_SOURCE_DIR}/src/include
)

include(GNUInstallDirs)
install(TARGETS libqgraph
  EXPORT qgraph-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT qgraph-targets
  FILE qgraph-config.cmake
  NAMESPACE qgraph::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qgraph
)
