cmake_minimum_required (VERSION 2.6)

project(mbm_client)

set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)

include_directories(
	${PROJECT_ROOT_DIR}/src)

file(GLOB_RECURSE SRC_FILES *.cc)

add_executable(mbm_client ${SRC_FILES})
target_link_libraries(mbm_client mbm) 
