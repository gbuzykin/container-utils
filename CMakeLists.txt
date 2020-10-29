cmake_minimum_required(VERSION 3.12)
project(Util_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__")
endif()

add_definitions("-D_STANDALONE")

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`
file(GLOB_RECURSE includes      include/*.h)
file(GLOB_RECURSE sources       src/*.cpp)
file(GLOB_RECURSE sources_tests tests/*.cpp)
# you can use set(sources src/main.cpp) etc if you don't want to
# use globing to find files automatically

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(Util_Tests
  .clang-format
  main.cpp
  ${includes}
  ${sources}
  ${sources_tests}
)

# this lets me include files relative to the root src dir with a <> pair
target_include_directories(Util_Tests PUBLIC include tests)
