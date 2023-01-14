cmake_minimum_required(VERSION 3.19)

project(MazeGenerator VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
	SFML
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 2.5.1
)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)	

add_executable(MazeGenerator ./src/Main.cpp 
					  ./src/Interface.cpp 
					  ./src/Maze.cpp 
					  ./src/MazeSolver.cpp
					  ./src/Render.cpp)

target_include_directories(MazeGenerator PUBLIC "${PROJECT_BINARY_DIR}/src")
target_link_libraries(MazeGenerator sfml-graphics)
				