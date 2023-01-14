#ifndef MAZE_SOLVERS_HPP
#define MAZE_SOLVERS_HPP

#include <random>
#include <iostream>

class Renderer;
class MazeSolver;

#include <SFML/Graphics.hpp>

#include "Maze.hpp"

// template hell just so that I only have one wrapper function to unactivate the window after the recursion finishes
template <class T, class F, class... Params>
static void start(T algo, F& obj, Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay, Params&&... params) {
    (obj.*algo)(maze, window, renderer, delay, std::forward<Params>(params)...);
    window.setActive(false);
}
 
class MazeSolver {
public:
    MazeSolver();

    void recursiveBacktrack(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay, int row, int col, Maze::Direction dir);
    void growingTree(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay);
    void ellers(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay);
    void recursiveDivision(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay, int row, int col, int width, int height, bool orientation);

    void update(sf::RenderWindow& window, Renderer& renderer, int delay);

    int irand(int min, int max);

    enum class Algorithm {
        RecursiveBacktrack,
        GrowingTree,
        Ellers,
        RecursiveDivision
    };

private:
    Maze::Direction getRandomDir();
    std::pair<sf::Vector2u, Maze::Direction> getCell(std::vector<std::pair<sf::Vector2u, Maze::Direction>>& cells);
    bool pickOrientation(int width, int height);

    std::random_device rd;
    std::default_random_engine rng;
    std::mt19937 gen{ rd() };
};

static void printAlgo(MazeSolver::Algorithm algo) {
    switch (algo) {
        case MazeSolver::Algorithm::RecursiveBacktrack:
            std::cout << "Backtrack" << std::endl;
            break;
        case MazeSolver::Algorithm::GrowingTree:
            std::cout << "Growing Tree" << std::endl;
            break;
        case MazeSolver::Algorithm::Ellers:
            std::cout << "Ellers" << std::endl;
            break;
    }
}

#endif /* MAZE_SOLVERS_HPP */