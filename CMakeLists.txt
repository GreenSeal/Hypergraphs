cmake_minimum_required(VERSION 3.16)
project(FM)

set(CMAKE_CXX_STANDARD 20)

add_executable(FM main.cpp)


file(GLOB BENCHMARKS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set benchmark_set/*.hgr)

foreach(BENCHMARK ${BENCHMARKS})
    list(APPEND BENCHMARKS_TARGETS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK}.part.2)
    message(${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK})
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK}.part.2
            BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK}.result ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK}.mod.result
            COMMAND FM ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK} -m
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/benchmark_set/${BENCHMARK}
    )
endforeach()
message(${BENCHMARKS_TARGETS})
add_library(BENCHMARKS_RUN_LIB INTERFACE)
target_sources(BENCHMARKS_RUN_LIB INTERFACE ${BENCHMARKS_TARGETS})
add_executable(BENCHMARKS_RUN main.cpp)
target_link_libraries(BENCHMARKS_RUN BENCHMARKS_RUN_LIB)
add_dependencies(BENCHMARKS_RUN FM)