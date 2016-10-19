cmake_minimum_required (VERSION 2.6)

#set(CMAKE_BUILD_TYPE Debug)

project(mbm_lib)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)

# file(GLOB_RECURSE SRC_FILES *.cc)

include_directories(
	${PROJECT_ROOT_DIR}/src)

file(GLOB_RECURSE SRC_FILES *.cc)

# Build the libraries
add_library(mbm ${SRC_FILES})
target_link_libraries(mbm rt) 
