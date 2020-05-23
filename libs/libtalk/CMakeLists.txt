add_library(libtalk
  src/ast.cpp
  src/char_id.cpp
  src/lexer.cpp
  src/parser.cpp
  src/utils.cpp
)

target_include_directories(libtalk
  PUBLIC include
  PRIVATE src
)

enable_clang_tidy(libtalk)
target_compile_features(libtalk PUBLIC cxx_std_17)

add_subdirectory(tests)