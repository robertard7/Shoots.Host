cmake_minimum_required(VERSION 3.20)

project(ShootsHost VERSION 0.1.0 LANGUAGES C CXX)

include(cmake/Options.cmake)
include(cmake/Warnings.cmake)

find_package(ShootsProvider CONFIG QUIET)
if (NOT ShootsProvider_FOUND)
  message(FATAL_ERROR
    "ShootsProvider package not found. Install Shoots.Provider and ensure ShootsProviderConfig.cmake is discoverable, "
    "or pass -DCMAKE_PREFIX_PATH=<prefix> where it is installed.")
endif()

add_executable(ShootsHost
  src/main.cpp
  src/http/error_envelopes.cpp
  src/jobs/job_store.cpp
  src/provider_bridge/provider_bridge.cpp
  src/routes/routes.cpp
  src/server/app_state.cpp
  src/server/http_server.cpp
  src/server/json_writer.cpp
)
set_target_properties(ShootsHost PROPERTIES OUTPUT_NAME shoots-host)

target_include_directories(ShootsHost PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/server"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/httplib"
)

target_link_libraries(ShootsHost PRIVATE Shoots::ShootsProvider)

target_compile_features(ShootsHost PRIVATE cxx_std_20)
shoots_host_apply_warnings(ShootsHost)

install(TARGETS ShootsHost RUNTIME DESTINATION bin)
install(FILES LICENSE THIRD_PARTY_NOTICES.md DESTINATION share/shoots-host)
install(PROGRAMS scripts/run_host.sh DESTINATION share/shoots-host)

if (SHOOTS_HOST_BUILD_TESTS)
  enable_testing()

  add_executable(HostGoldenSnapshots
    tests/contract/host_golden_snapshots.cpp
    src/http/error_envelopes.cpp
    src/jobs/job_store.cpp
    src/provider_bridge/provider_bridge.cpp
    src/routes/routes.cpp
    src/server/app_state.cpp
    src/server/json_writer.cpp
  )

  target_include_directories(HostGoldenSnapshots PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/httplib"
  )
  target_link_libraries(HostGoldenSnapshots PRIVATE Shoots::ShootsProvider)
  target_compile_features(HostGoldenSnapshots PRIVATE cxx_std_20)
  target_compile_definitions(HostGoldenSnapshots PRIVATE SHOOTS_HOST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
  shoots_host_apply_warnings(HostGoldenSnapshots)

  add_test(NAME HostGoldenSnapshots COMMAND HostGoldenSnapshots)
endif()
