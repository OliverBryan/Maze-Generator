#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "MazeSolver.hpp"
#include <vector>

// instance data for the animation
struct RunInfo {
    MazeSolver::Algorithm algo;
    sf::Vector2u mazeSize;
    int delay;
};

namespace ui {
    // self explanatory
    static void printRunInfo(RunInfo info) {
        printAlgo(info.algo);
        std::cout << info.delay << "\n" << info.mazeSize.x << " " << info.mazeSize.y << std::endl;
    }

    // gets user input from the console to configure the next animation
    RunInfo configureRun(RunInfo previous);

    // loads automated runs from the specified file 
    std::vector<RunInfo> loadRunsFromFile(const std::string& fileName);
};

#endif /* INTERFACE_HPP */