cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(ClothSimulation)

find_package(OpenGL REQUIRED)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
	${CMAKE_BINARY_DIR}/bin CACHE PATH "Executable directory"
)

add_subdirectory(Dependencies)
add_subdirectory(src)
