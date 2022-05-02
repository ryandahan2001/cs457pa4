cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(proj1)

include_directories(include)

set(srcFolder src)

add_executable(proj1 src/driver.cpp)

install(TARGETS proj1
	RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/devel/bin
	LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/devel/lib
	ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/devel/lib/static)
	
install (DIRECTORY ${PROJECT_SOURCE_DIR}/devel/bin
	DESTINATION ${PROJECT_SOURCE_DIR}/devel/lib
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")