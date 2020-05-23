add_executable(libtalk_tests
  lexer_tests.cpp
  expr_parser_test.cpp
)

target_link_libraries(libtalk_tests 
  doctest_main 
  libtalk
)

enable_clang_tidy(libtalk_tests)

add_test(libtalk_tests libtalk_tests)
