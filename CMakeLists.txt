cmake_minimum_required(VERSION 3.0.0)
project(blitz_query_cpp VERSION 0.1.0 LANGUAGES C CXX)
set(CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
add_compile_options(/W4 )
set(CMAKE_CXX_FLAGS_RELEASE "/O2")

else()
add_compile_options(-Wall -Wextra -Wpedantic )
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# add_compile_definitions(GTEST_IS_THREADSAFE=0)
# add_compile_definitions(GTEST_HAS_PTHREAD=0)

set(PostgreSQL_ADDITIONAL_VERSIONS "14" "15" "16")
find_package(PostgreSQL REQUIRED)

include(CTest)
enable_testing()
include_directories(
    "inc"
    "3rd-party/googletest/googletest/include" 
    "3rd-party/googletest/googletest" 
    "3rd-party/googletest/googlemock/include"
    "3rd-party/googletest/googlemock"
    ${PostgreSQL_INCLUDE_DIRS}
    )

file(GLOB lib_SRC
     "*lib/*.h"
     "src/*.cpp"
)

file(GLOB test_SRC
     "tests/unit/*.cpp"
     "3rd-party/googletest/googlemock/src/gmock_main.cc"
     "3rd-party/googletest/googletest/src/gtest-all.cc"
     "3rd-party/googletest/googlemock/src/gmock-all.cc"
)


add_library(blitz_query_cpp_lib STATIC  ${lib_SRC})

add_executable(test_runner ${lib_SRC} ${test_SRC}) 

add_test(NAME blitz_query_cpp_test COMMAND test_runner)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
