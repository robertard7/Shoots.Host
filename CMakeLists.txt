cmake_minimum_required(VERSION 3.20)

project(ShootsHost VERSION 0.1.0 LANGUAGES C CXX)

include(cmake/Options.cmake)
include(cmake/Warnings.cmake)

find_package(ShootsProvider CONFIG REQUIRED)

add_executable(ShootsHost
  src/main.cpp
  src/routes/routes.cpp
  src/server/app_state.cpp
  src/server/http_server.cpp
  src/server/json_writer.cpp
)

target_include_directories(ShootsHost PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/server"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib"
)

target_link_libraries(ShootsHost PRIVATE Shoots::ShootsProvider)

target_compile_features(ShootsHost PRIVATE cxx_std_20)
shoots_host_apply_warnings(ShootsHost)
