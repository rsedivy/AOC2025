cmake_minimum_required(VERSION 3.15)
project(AOC2025 C)

option(DAY "Build a single day's solution (e.g. day1)" "")

file(GLOB day_dirs RELATIVE ${CMAKE_SOURCE_DIR} day*/)

if(DAY)
  set(selected_days ${DAY}/)
else()
  set(selected_days ${day_dirs})
endif()

foreach(day ${selected_days})
  file(GLOB sources "${day}/*.c")

  string(REPLACE "/" "" exe_name ${day})

  # Set output directory to day[n]/bin/
  set(output_dir "${CMAKE_SOURCE_DIR}/${day}/bin")
  file(MAKE_DIRECTORY ${output_dir})

  add_executable(${exe_name} ${sources})

  if(EXISTS "${CMAKE_SOURCE_DIR}/${day}/CMakeOptions.txt")
    include("${CMAKE_SOURCE_DIR}/${day}/CMakeOptions.txt")
  endif()

  # The target's RUNTIME_OUTPUT_DIRECTORY sets where the executable will be placed
  set_target_properties(${exe_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
    )
endforeach()
