#ifndef MAZE_HPP
#define MAZE_HPP

#include <vector>
#include <iostream>

// forward declarations
class Renderer;

// more forward declarations
namespace sf {
    template <class T> class Vector2;
    using Vector2u = Vector2<unsigned int>;

    class Color;
}

// represents the maze to be generated 
class Maze {
public:
    // constructor
    Maze(const int rows, const int cols);

    // this allows us to easily print a maze
    friend std::ostream& operator<<(std::ostream& os, const Maze& maze);

    friend class Renderer;

    // assignment operator overloads
    Maze& operator=(const Maze& other);

    // resizes the maze to the new specified size 
    void resize(const int rows, const int cols);

    //
    enum class Direction {
        Up,
        Down,
        Left,
        Right,
        None
    };

    sf::Vector2u getSize();

    void toggleWall(const int row, const int col, Direction dir, Renderer& renderer);
    void toggleWall(const int row, const int col, Direction dir, Renderer& renderer, sf::Color cellFill, sf::Color wallFill);

    bool isVisited(const int row, const int col, Direction dir);

    void removeWalls();

private:
    std::vector<std::vector<bool>> grid;

    void initialize();

    bool isVisitedImpl(const int row, const int col);

    int rows;
    int cols;
};

static void printDir(Maze::Direction dir) {
    switch (dir) {
        case Maze::Direction::Up:
            std::cout << "Up" << std::endl;
            break;
        case Maze::Direction::Down:
            std::cout << "Down" << std::endl;
            break;
        case Maze::Direction::Left:
            std::cout << "Left" << std::endl;
            break;
        case Maze::Direction::Right:
            std::cout << "Right" << std::endl;
            break;
    }
}

#endif /* MAZE_HPP */