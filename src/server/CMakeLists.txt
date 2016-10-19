cmake_minimum_required (VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)


project(mbm_server)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)


find_package(Threads REQUIRED)

include_directories(
	${PROJECT_ROOT_DIR}/src)

file(GLOB_RECURSE SRC_FILES *.cc)

# Build the libraries
add_executable(mbm_server ${SRC_FILES})


target_link_libraries(mbm_server
	mbm
	${CMAKE_THREAD_LIBS_INIT})