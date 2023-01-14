#include "Interface.hpp"
#include <fstream>

namespace ui {
    // invisible namespace for "private" functions
    namespace {
        // gets integer input through the console within the specified bounds 
        int getInputInBounds(int min, int max) {
            std::string s;
            int i;
            
            // loop until input is within bounds
            while (true) {
                try {
                    std::getline(std::cin, s);
                    i = std::stoi(s);

                    if (i < min || i > max) 
                        throw std::invalid_argument("Input must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "]");

                    break;
                } catch (std::invalid_argument) {
                    std::cout << "Invalid input, please re enter" << std::endl;
                    continue;
                }
            }

            return i;
        }

        // gets console input to determine the algorithm to animate
        MazeSolver::Algorithm getAlgorithm() {
            std::cout << "Algorithms:\nRecursive Backtrack(1)\nGrowing Tree(2)\nEller's Algorithm(3)\nRecursive Division(4)" << std::endl;
            int algoChoice = getInputInBounds(1, 4);

            switch (algoChoice) {
                case 1:
                    return MazeSolver::Algorithm::RecursiveBacktrack;
                case 2:
                    return MazeSolver::Algorithm::GrowingTree;
                case 3:
                    return MazeSolver::Algorithm::Ellers;
                case 4:
                    return MazeSolver::Algorithm::RecursiveDivision;
            }

            //unreachable
            return MazeSolver::Algorithm::RecursiveBacktrack;
        }

        // gets console input to determine the size of the maze
        sf::Vector2u getMazeSize() {
            std::cout << "Enter the width of the maze (must be between 3 and 60)" << std::endl;
            int width = getInputInBounds(3, 60);

            std::cout << "Enter the height of the maze (must be between 3 and 60)" << std::endl;
            int height = getInputInBounds(3, 60);

            return sf::Vector2u(width, height);
        }

        // gets console input to determine the delay between window draws
        int getDelay() {
            std::cout << "Enter the delay (in milliseconds between 0 and 2000) between frames (high delay = slower speed, use a lower delay for larger mazes)" << std::endl;
            return getInputInBounds(0, 2000);
        }
    }

    // see header
    RunInfo configureRun(RunInfo previous) {
        // print menu and initialize new run info object to the previous one (saving settings of previous animation)
        std::cout << "\nMenu: \nSet Algorithm (1)\nSet Maze Size (2)\nSet Speed(3)\nRun(4)\nExit(5)" << std::endl;
        RunInfo info {previous};

        // get input from user and update corresponding properties of the RunInfo object
        int choice = getInputInBounds(1, 5);
        while (choice != 4 && choice != 5) {
            switch (choice) {
                case 1:
                    info.algo = getAlgorithm();
                    break;
                case 2:
                    info.mazeSize = getMazeSize();
                    break;
                case 3:
                    info.delay = getDelay();
                    break;
            }
            
            std::cout << "Menu: \nSet Algorithm (1)\nSet Maze Size (2)\nSet Speed(3)\nRun(4)\nExit(5)" << std::endl;
            choice = getInputInBounds(1, 5);
        }
        
        if (choice == 5)
            std::exit(1);

        return info;
    }

    // load lines the input file (assumes that lines are all composed of a single 32-bit integer)
    std::vector<int> getLines(std::fstream& file) {
        std::vector<int> lines;
        std::string line;

        // read file and parse data
        try {
            while (std::getline(file, line))
                lines.push_back(std::stoi(line));
        } catch (std::invalid_argument e) {
            // if one line fails the whole operation fails because I'm too lazy to make a better system
            std::cout << "Error: could not parse run.dat (Interface.cpp: line 90))" << std::endl;
            return {};
        }
        
        // reverse the list to make processing easier;
        std::reverse(lines.begin(), lines.end());
        
        return lines;
    }

    // see header
    std::vector<RunInfo> loadRunsFromFile(const std::string& fileName) {
        std::vector<RunInfo> runs;

        // attempt to open the file
        std::fstream file;
        file.open(fileName);

        // if it doesn't work then oh well we tried
        if (!file) {
            std::cout << "Could not open " << fileName << std::endl;
            return {};
        }

        auto lines = getLines(file);
        RunInfo info;

        // if there isn't the right number of lines to construct a whole number of RunInfo objects then oh well we tried 
        if (lines.size() % 4 != 0) {
            std::cout << "Error: Could not parse run.dat (Interface.cpp: line 113))" << std::endl;
            return {};
        }

        // attempt to construct RunInfo objects based on the data from the file
        while (lines.size() > 0) {
            int algo = lines.back();
            lines.pop_back();
            
            MazeSolver::Algorithm algoType;
            switch (algo) {
                case 1:
                    algoType = MazeSolver::Algorithm::RecursiveBacktrack;
                    break;
                case 2:
                    algoType = MazeSolver::Algorithm::GrowingTree;
                    break;
                case 3:
                    algoType = MazeSolver::Algorithm::Ellers;
                    break;
                case 4:
                    algoType = MazeSolver::Algorithm::RecursiveDivision;
                    break;
                default:
                    // if the input isn't in bounds then oh well we tried
                    std::cout << "Error: Could not parse run.dat (Interface.cpp: line 132))" << std::endl;
                    return {};
            }

            unsigned int rows = lines.back();
            lines.pop_back();

            unsigned int cols = lines.back();
            lines.pop_back();

            // if input isn't in bounds at least we tried
            if (rows < 3 || cols < 3 || rows > 60 || cols > 60) {
                std::cout << "Error: Could not parse run.dat (Interface.cpp: line 143))" << std::endl;
                return {};
            }

            int delay = lines.back();
            lines.pop_back();

            RunInfo info {algoType, sf::Vector2u(rows, cols), delay};
            runs.push_back(info);
        }

        return runs;
    }
}