#include "Maze.hpp"
#include "Render.hpp"

#include <SFML/Graphics.hpp>

Maze::Maze(const int rows, const int cols) : rows(rows), cols(cols) {
    initialize();
}

std::ostream& operator<<(std::ostream& os, const Maze& maze) {
    for (int i = 0; i < maze.grid.size(); i++) {
        for (int j = 0; j < maze.grid[0].size(); j++)
            os << maze.grid[i][j];
        
        os << "\n";
    }
    
    return os;
}

void Maze::resize(const int rows, const int cols) {
    this->rows = rows;
    this->cols = cols;
    initialize();
}

void Maze::initialize() {
    grid.resize(rows * 2 + 1); 
    for (int i = 0; i < rows * 2 + 1; i++) {
        grid[i] = std::vector<bool>(cols * 2 + 1);
        for (int j = 0; j < grid[i].size(); j++)
            if (j % 2 != 0 && i % 2 != 0)
                grid[i][j] = true;
    }
}

Maze& Maze::operator=(const Maze& other) {
    if (this != &other && other.grid.size() > 0) {
        rows = other.rows;
        cols = other.cols;
        for (int i = 0; i < other.grid.size(); i++)
            for (int j = 0; j < other.grid[0].size(); j++)
                grid[i][j] = other.grid[i][j];
    }
    return *this;
}

void Maze::toggleWall(const int row, const int col, Direction dir, Renderer& renderer, sf::Color cellFill, sf::Color wallFill) {
    renderer.toggleCell(row * 2 + 1, col * 2 + 1, cellFill);
    switch (dir) {
        case Direction::Up:
            grid[row * 2][col * 2 + 1] = (grid[row * 2][col * 2 + 1] ? false : true);
            renderer.toggleCell(row * 2 - 1, col * 2 + 1, cellFill);
            renderer.toggleWall(row * 2, col * 2 + 1, wallFill);
            break;
        case Direction::Down:
            grid[row * 2 + 2][col * 2 + 1] = (grid[row * 2 + 2][col * 2 + 1] ? false : true);
            renderer.toggleCell(row * 2 + 3, col * 2 + 1, cellFill);
            renderer.toggleWall(row * 2 + 2, col * 2 + 1, wallFill);
            break;
        case Direction::Left:
            grid[row * 2 + 1][col * 2] = (grid[row * 2 + 1][col * 2] ? false : true);
            renderer.toggleCell(row * 2 + 1, col * 2 - 1, cellFill);
            renderer.toggleWall(row * 2 + 1, col * 2, wallFill);
            break;
        case Direction::Right:
            grid[row * 2 + 1][col * 2 + 2] = (grid[row * 2 + 1][col * 2 + 2] ? false : true);
            renderer.toggleCell(row * 2 + 1, col * 2 + 3, cellFill);
            renderer.toggleWall(row * 2 + 1, col * 2 + 2, wallFill);
            break;
        case Direction::None:
            return;
    }
}

void Maze::toggleWall(const int row, const int col, Direction dir, Renderer& renderer) {
    toggleWall(row, col, dir, renderer, sf::Color(242, 94, 94), sf::Color(242, 94, 94));
}

bool Maze::isVisitedImpl(const int row, const int col) {
    return grid[row * 2][col * 2 + 1] ||
           grid[row * 2 + 2][col * 2 + 1] || 
           grid[row * 2 + 1][col * 2] ||
           grid[row * 2 + 1][col * 2 + 2];
}

bool Maze::isVisited(const int row, const int col, Direction dir) {
    switch (dir) {
        case Direction::Up:
            if (row == 0) return true;
            else return isVisitedImpl(row - 1, col);
        case Direction::Down:
            if (row == rows - 1) return true;
            else return isVisitedImpl(row + 1, col);
        case Direction::Left:
            if (col == 0) return true;
            else return isVisitedImpl(row, col - 1);
        case Direction::Right:
            if (col == cols - 1) return true;
            else return isVisitedImpl(row, col + 1);
    }
    return false;
}

sf::Vector2u Maze::getSize() {
    return sf::Vector2u(rows, cols);
}

void Maze::removeWalls() {
    for (int i = 0; i < rows * 2 + 1; i++) {
        for (int j = 0; j < grid[i].size(); j++)
            if ((j % 2 == 0 || i % 2 == 0) && (j != 0 && i != 0 && j != cols * 2 && i != rows * 2))
                grid[i][j] = true;
    }
}