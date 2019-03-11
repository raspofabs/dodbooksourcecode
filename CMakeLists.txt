cmake_minimum_required(VERSION 3.6)

project(dodbook C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

function(add_benchmark name)
  add_executable(${name} ${name}.cpp)
  add_custom_target(run_${name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${name}
    DEPENDS ${name})
  set(run_all_benchmarks ${run_all_benchmarks} run_${name} PARENT_SCOPE)
endfunction()

add_benchmark(KeyLookup)
add_benchmark(basictheory)
add_benchmark(branch_prediction)
add_benchmark(cachesizeeffect)
add_benchmark(fsm)
add_benchmark(hotcold)
add_benchmark(linkedlists)
add_benchmark(matrixtranspose)
add_benchmark(modifying_memory)
add_benchmark(readingtests)
add_benchmark(simd_test)
add_benchmark(speculativewaste)
if(OPENMP_FOUND)
  add_benchmark(false_sharing)
endif()

add_custom_target(run_all DEPENDS ${run_all_benchmarks})
