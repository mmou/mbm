cmake_minimum_required (VERSION 2.6)

project(mbm_client)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)


file(GLOB_RECURSE SRC_FILES *.cc)

# Build the libraries
add_executable(mbm_client ${SRC_FILES})
