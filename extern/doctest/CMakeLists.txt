add_library(doctest_main doctest_main.cpp)

target_include_directories(doctest_main 
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)