cmake_minimum_required(VERSION 3.20)

project(ShootsHost VERSION 0.1.0 LANGUAGES C CXX)

find_package(ShootsProvider CONFIG REQUIRED)

add_executable(ShootsHost
  src/main.cpp
  src/server/http_server.cpp
)

target_include_directories(ShootsHost PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/server"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib"
)

target_link_libraries(ShootsHost PRIVATE Shoots::ShootsProvider)

target_compile_features(ShootsHost PRIVATE cxx_std_20)

if (MSVC)
  target_compile_options(ShootsHost PRIVATE /W4)
else()
  target_compile_options(ShootsHost PRIVATE -Wall -Wextra -Wpedantic)
endif()

