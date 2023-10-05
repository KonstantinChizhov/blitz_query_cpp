cmake_minimum_required(VERSION 3.0.0)
project(blitz_query_cpp VERSION 0.1.0 LANGUAGES C CXX)
set(CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
add_compile_options(/W4 /WX)
else()
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include(CTest)
enable_testing()
include_directories(
    "inc"
    "3rd-party/googletest/googletest/include" 
    "3rd-party/googletest/googletest" 
    "3rd-party/googletest/googlemock/include"
    "3rd-party/googletest/googlemock")

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

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
