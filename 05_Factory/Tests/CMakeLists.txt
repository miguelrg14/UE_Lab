# Builds the engine-independent part of the Factory project (Source/factory/Core) outside Unreal:
#   - factory_tests : GoogleTest unit tests
#   - factory_sim   : console simulation used as a smoke test in CI
cmake_minimum_required(VERSION 3.20)
project(FactoryCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FACTORY_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Project/Source/factory/Core)

if(MSVC)
  add_compile_options(/W4 /WX /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror)
endif()

add_library(factory_core INTERFACE)
target_include_directories(factory_core INTERFACE ${FACTORY_CORE_DIR})

add_executable(factory_sim FactorySim.cpp)
target_link_libraries(factory_sim PRIVATE factory_core)

option(FACTORY_BUILD_TESTS "Build the unit tests (uses or downloads GoogleTest)" ON)
if(FACTORY_BUILD_TESTS)
  # Use an installed GoogleTest if there is one, otherwise fetch it.
  find_package(GTest CONFIG QUIET)
  if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.15.2
      GIT_SHALLOW TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

  enable_testing()
  add_executable(factory_tests FactoryRulesTests.cpp FactorySimulationTests.cpp)
  target_link_libraries(factory_tests PRIVATE factory_core GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(factory_tests)

  # Smoke test: the console simulation must run and exit with code 0.
  add_test(NAME factory_sim_smoke COMMAND factory_sim --lines 3 --steps 200 --seed 42 --quiet)
endif()
