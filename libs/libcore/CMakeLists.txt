add_library(libcore
    src/environment.cpp
)

target_include_directories(libcore
  PUBLIC
    include
  PRIVATE
    src
)

enable_clang_tidy(libcore)
target_compile_features(libcore PUBLIC cxx_std_17)

add_subdirectory(tests)