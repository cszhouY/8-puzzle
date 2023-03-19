cmake_minimum_required (VERSION 3.25)
project (8-puzzle)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(./src DIR_SRCS)
include_directories(./gvl)
link_directories(./gvl)
add_executable(8-puzzle ${DIR_SRCS})
target_link_libraries(8-puzzle gvl)
