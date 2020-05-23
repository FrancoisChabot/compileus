cmake_minimum_required(VERSION 3.13)

project(compileus)

option(COMPILEUS_VALIDATE "Enables aggressive validation" OFF)

find_package(spdlog)

if(COMPILEUS_VALIDATE)
  message(STATUS "Compileus: Enabling aggressive validation")

  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
  set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")

  function(enable_clang_tidy TARGET)
    set_target_properties(${TARGET} 
      PROPERTIES
        CXX_CLANG_TIDY "clang-tidy;--format-style=file"
    )
  endfunction(enable_clang_tidy)
else()
  function(enable_clang_tidy TARGET)
  endfunction(enable_clang_tidy)
endif()


add_subdirectory(extern/doctest)

enable_testing()
add_subdirectory(libs)

add_subdirectory(apps/talkc)
add_subdirectory(apps/compileus)
