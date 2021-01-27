#=========================================================================
# CMake script for PA4
#=========================================================================
# To add new implementations or new tests, simply append your new file to
# the corresponding list.
#
# Author: Yanghui Ou 
# Modified by Eric Hall from Yanghui Ou's CMakeLists.txt for ECE 2400
#   Created: Sep. 26, 2019
#   Modified: Sep 29, 2020
#

set(TEST_FILES
# Smoke tests
  eigen_smoke_test.cc
  syntax_smoke_test.cc
  boost_smoke_test.cc

# Directed tests
  node_directed_test.cc

# Random tests
)

set (EVAL_FILES
  
)

set (DATA_FILES
  
)

#=========================================================================
# !!!!DO NOT TOUCH THE FOLLOWING CODE!!!!
#=========================================================================

#-------------------------------------------------------------------------
# remove_extension
#-------------------------------------------------------------------------
# Removes the file extension. For example:
# remove_extension(IMPL, "pow-iter.c") stores "pow-iter" into IMPL.

function(remove_extension VARNAME FILE_NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" NAME_WITHOUT_EXT ${FILE_NAME})
  set(${VARNAME} "${NAME_WITHOUT_EXT}" PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Set up environment
#-------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0)
enable_language(CXX)
enable_testing()
project(supranode)

# Set and cache the build type
set(default_build_type "debug")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "debug" "eval" )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "${default_build_type}")
endif()
message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'")

# Flags for g++
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wvla -pedantic-errors -Werror -std=c++20 -fconcepts")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_EVAL "-O3")

# Path to this project's source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(EVAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/eval")
set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Include path to this project's header files
include_directories(${SRC_DIR})

#-------------------------------------------------------------------------
# Include external packages
#-------------------------------------------------------------------------
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package (Boost REQUIRED)
if ( ${Boost_FOUND} )
  include_directories( ${Boost_INCLUDE_DIRS} )
endif()

#-------------------------------------------------------------------------
# Test targets
#-------------------------------------------------------------------------

add_custom_target(check COMMAND ctest "--timeout" "10")

foreach(TEST_FILE ${TEST_FILES})

  remove_extension(TEST_BIN ${TEST_FILE})
  set(TEST_SRC "${TEST_DIR}/${TEST_FILE}")

  # Add test
  add_executable(${TEST_BIN} EXCLUDE_FROM_ALL ${TEST_SRC})
  target_link_libraries(${TEST_BIN} Eigen3::Eigen )
  add_dependencies(check ${TEST_BIN})
  add_test(${TEST_BIN} ${TEST_BIN})

endforeach(TEST_FILE)

#-------------------------------------------------------------------------
# Evaluation targets
#-------------------------------------------------------------------------

add_custom_target(eval)

foreach(EVAL_FILE ${EVAL_FILES})

  remove_extension(EVAL_BIN ${EVAL_FILE})
  set(EVAL_SRC "${EVAL_DIR}/${EVAL_FILE}")

  # Add eval
  add_executable(${EVAL_BIN} EXCLUDE_FROM_ALL ${EVAL_SRC})
  target_link_libraries(${EVAL_BIN} Eigen3::Eigen )
  add_dependencies(eval ${EVAL_BIN})

endforeach(EVAL_FILE)

#-------------------------------------------------------------------------
# Data gathering targets
#-------------------------------------------------------------------------

add_custom_target(data)

foreach(DATA_FILE ${DATA_FILES})

  remove_extension(DATA_BIN ${DATA_FILE})
  set(DATA_SRC "${DATA_DIR}/${DATA_FILE}")

  # Add data
  add_executable(${DATA_BIN} EXCLUDE_FROM_ALL ${DATA_SRC})
  target_link_libraries(${DATA_BIN} Eigen3::Eigen )
  add_dependencies(data ${DATA_BIN})

endforeach(DATA_FILE)

#-------------------------------------------------------------------------
# Code coverage target
#-------------------------------------------------------------------------

add_custom_target(coverage
  COMMAND lcov --capture --directory . --output-file coverage.info
  COMMAND genhtml coverage.info --output-directory coverage-html
)

#-------------------------------------------------------------------------
# Mem check target
#-------------------------------------------------------------------------

add_custom_target(memcheck
  COMMAND echo ""
  COMMAND echo "========================================================="
  COMMAND echo " memcheck passed!"
  COMMAND echo "========================================================="
  COMMAND echo ""
)

set(TEST_ALL_FILES ${TEST_FILES} ${EVAL_FILES} ${DATA_FILES})

foreach(TEST_FILE ${TEST_ALL_FILES})

  remove_extension(TEST_BIN ${TEST_FILE})
  add_custom_target(memcheck-${TEST_BIN}
    COMMAND mkdir -p "memcheck-log"
    COMMAND valgrind "--trace-children=yes"
                     "--leak-check=full"
                     "--error-exitcode=1"
                     "--undef-value-errors=no"
                     "--log-file=memcheck-log/${TEST_BIN}.log"
                     "${CMAKE_CURRENT_BINARY_DIR}/${TEST_BIN}"
                     ">" "memcheck-log/${test_bin}.stdout"
    COMMAND sed "-r" "\"s/\\x1B\\[([0-9]{1,2}(;[0-9]{1,2})?)?[mGK]//g\""
                "memcheck-log/${test_bin}.stdout"
                ">" "memcheck-log/${test_bin}.stdout"
    DEPENDS ${TEST_BIN}
  )
  add_dependencies(memcheck memcheck-${TEST_BIN})

endforeach(TEST_FILE)
