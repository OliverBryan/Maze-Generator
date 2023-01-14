#include "MazeSolver.hpp"
#include "Maze.hpp"
#include "Render.hpp"
#include <algorithm>
#include <chrono>
#include <thread>
#include <map>

MazeSolver::MazeSolver() : rng(rd()) {}

sf::Vector2u changePosition(int row, int col, Maze::Direction dir) {
    sf::Vector2u pos(row, col);
    
    switch (dir) {
        case Maze::Direction::Up:
            pos.x -= 1;
            break;
        case Maze::Direction::Down:
            pos.x += 1;
            break;
        case Maze::Direction::Left:
            pos.y -= 1;
            break;
        case Maze::Direction::Right:
            pos.y += 1;
            break;
    }

    return pos;
}

void backtrack(int row, int col, sf::RenderWindow& window, Renderer& renderer, Maze::Direction dir) {
    switch (dir) {
        case Maze::Direction::Up:
            // going down
            renderer.toggleWall(row * 2 + 2, col * 2 + 1, sf::Color::White);
            break;
        case Maze::Direction::Down:
            renderer.toggleWall(row * 2, col * 2 + 1, sf::Color::White);
            break;
        case Maze::Direction::Left:
            renderer.toggleWall(row * 2 + 1, col * 2 + 2, sf::Color::White);
            break;
        case Maze::Direction::Right:
            renderer.toggleWall(row * 2 + 1, col * 2, sf::Color::White);
            break;
    }
}

void MazeSolver::update(sf::RenderWindow& window, Renderer& renderer, int delay) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    window.clear(sf::Color::White);
    renderer.draw(window);
    window.display();
}

void MazeSolver::recursiveBacktrack(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay, int row, int col, Maze::Direction dir) {
    std::vector<Maze::Direction> dirs {Maze::Direction::Up, 
                                       Maze::Direction::Left, 
                                       Maze::Direction::Down, 
                                       Maze::Direction::Right};

    std::shuffle(std::begin(dirs), std::end(dirs), rng);

    for (const auto& dir : dirs) {
        if (!maze.isVisited(row, col, dir)) {
            maze.toggleWall(row, col, dir, renderer);
            auto pos = changePosition(row, col, dir);

            update(window, renderer, delay);
            recursiveBacktrack(maze, window, renderer, delay, pos.x, pos.y, dir);
        }
    }

    if (dir != Maze::Direction::None) {
        renderer.toggleCell(row * 2 + 1, col * 2 + 1, sf::Color::White);
        backtrack(row, col, window, renderer, dir);
        update(window, renderer, delay);
    }
    else {
        renderer.toggleCell(row * 2 + 1, col * 2 + 1, sf::Color::White);
        update(window, renderer, delay);
    }
}

Maze::Direction MazeSolver::getRandomDir() {
    std::vector<Maze::Direction> dirs {Maze::Direction::Up, 
                                       Maze::Direction::Left, 
                                       Maze::Direction::Down, 
                                       Maze::Direction::Right};

    std::shuffle(std::begin(dirs), std::end(dirs), rng);

    return dirs[0];
}

int MazeSolver::irand(int min, int max) {
    if (min == max)
        return min;
	std::uniform_int_distribution<> dis{ min, max };

	return dis(gen);
}

bool hasUnvisitedNeighbors(int row, int col, Maze& maze) {
    std::vector<Maze::Direction> dirs {Maze::Direction::Up, 
                                       Maze::Direction::Left, 
                                       Maze::Direction::Down, 
                                       Maze::Direction::Right};
    
    for (Maze::Direction dir : dirs)
        if (!maze.isVisited(row, col, dir))
            return true;
    return false;
}

std::pair<sf::Vector2u, Maze::Direction> MazeSolver::getCell(std::vector<std::pair<sf::Vector2u, Maze::Direction>>& cells) {
   return cells[irand(0, cells.size() - 1)];
}

void MazeSolver::growingTree(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay) {
    std::vector<std::pair<sf::Vector2u, Maze::Direction>> cells;
    
    cells.push_back(std::make_pair(sf::Vector2u(irand(0, maze.getSize().x - 1), irand(0, maze.getSize().y - 1)), Maze::Direction::None));
    while (!cells.empty()) {
        auto cell = getCell(cells);

        if (hasUnvisitedNeighbors(cell.first.x, cell.first.y, maze)) {
            std::vector<Maze::Direction> dirs {Maze::Direction::Up, 
                                       Maze::Direction::Left, 
                                       Maze::Direction::Down, 
                                       Maze::Direction::Right};

            std::shuffle(std::begin(dirs), std::end(dirs), rng);
            
            for (Maze::Direction dir : dirs) {
                if (!maze.isVisited(cell.first.x, cell.first.y, dir)) {
                    maze.toggleWall(cell.first.x, cell.first.y, dir, renderer);
                    cells.push_back(std::make_pair(changePosition(cell.first.x, cell.first.y, dir), dir));
                    update(window, renderer, delay);
                    break;
                }
            }
        }
        else {
            cells.erase(std::find(cells.begin(), cells.end(), cell));
            renderer.toggleCell(cell.first.x * 2 + 1, cell.first.y * 2 + 1, sf::Color::White);
            backtrack(cell.first.x, cell.first.y, window, renderer, cell.second);
            update(window, renderer, delay);
        }
    }
}

// necessary to compare Vector2
template <class T>
struct less {
    bool operator()(const sf::Vector2<T>& one, const sf::Vector2<T>& two) const {
        return one.y < two.y;
    }
};

void printSets(std::map<unsigned int, std::vector<sf::Vector2u>>& sets) {
    std::cout << sets.size() << std::endl;
    for (const auto& [key, value] : sets) {
        std::cout << key << ": ";
        for (const auto& v : value)
            std::cout << "(" << v.x << ", " << v.y << ") ";
    }
}

void mergeSets(std::map<sf::Vector2u, unsigned int, less<unsigned int>>& cellSets, std::map<unsigned int, std::vector<sf::Vector2u>>& sets, unsigned int set1, unsigned int set2) {
    for (sf::Vector2u cell : sets.at(set2)) {
        cellSets.at(cell) = set1;
        sets.at(set1).push_back(cell);
    }
    
    sets.erase(set2);
}

void MazeSolver::ellers(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay) {
    std::map<sf::Vector2u, unsigned int, less<unsigned int>> cellSets;
    std::map<unsigned int, std::vector<sf::Vector2u>> sets;
    int setCounter = 1;
    for (int row = 0; row < maze.getSize().x; row++) {
        for (int col = 0; col < maze.getSize().y; col++) {
            if (cellSets.find(sf::Vector2u(row, col)) == cellSets.end()) {
                cellSets.insert(std::make_pair(sf::Vector2u(row, col), setCounter));
                
                if (sets.find(setCounter) != sets.end())
                    sets.at(setCounter).push_back(sf::Vector2u(row, col));
                else sets.insert(std::make_pair(setCounter, std::vector {sf::Vector2u(row, col)}));

                setCounter++;
            }
        }

        for (int col = 0; col < maze.getSize().y - 1; col++) {
            if (row != maze.getSize().x - 1 && irand(0, 100) > 50 && (cellSets.at(sf::Vector2u(row, col)) != cellSets.at(sf::Vector2u(row, col + 1))))  {
                maze.toggleWall(row, col, Maze::Direction::Right, renderer);
                mergeSets(cellSets, sets, cellSets.at(sf::Vector2u(row, col)), cellSets.at(sf::Vector2u(row, col + 1)));
                update(window, renderer, delay);
            }
            else if (row == maze.getSize().x - 1) {
                if (cellSets.at(sf::Vector2u(row, col)) != cellSets.at(sf::Vector2u(row, col + 1))) {
                    maze.toggleWall(row, col, Maze::Direction::Right, renderer);
                    mergeSets(cellSets, sets, cellSets.at(sf::Vector2u(row, col)), cellSets.at(sf::Vector2u(row, col + 1)));
                    update(window, renderer, delay);
                }
            }
        }

        std::map<sf::Vector2u, unsigned int, less<unsigned int>> newCellSets;
        std::map<unsigned int, std::vector<sf::Vector2u>> newSets;
        if (row != maze.getSize().x - 1) {
            for (const auto& [key, value] : sets) {
                int iters = irand(1, value.size());
                for (int i = 0; i < iters; i++) {
                    sf::Vector2u connection = value[irand(0, value.size() - 1)];
                    if (!maze.isVisited(row, connection.y, Maze::Direction::Down)) {
                        maze.toggleWall(row, connection.y, Maze::Direction::Down, renderer);
                        newCellSets.insert(std::make_pair(sf::Vector2u(row + 1, connection.y), key));
                        if (newSets.find(key) != newSets.end())
                            newSets.at(key).push_back(sf::Vector2u(row + 1, connection.y));
                        else newSets.insert(std::make_pair(key, std::vector {sf::Vector2u(row + 1, connection.y)}));
                        update(window, renderer, delay);
                    }
                }
            }
        }

        cellSets = newCellSets;
        sets = newSets;

        for (int col = 0; col < maze.getSize().y; col++) {
            renderer.toggleCell(row * 2 + 1, col * 2 + 1, sf::Color::White);
            if (col != maze.getSize().y - 1)
                renderer.toggleIf(row * 2 + 1, col * 2 + 2, sf::Color::White, sf::Color(242, 94, 94));
            if (row != maze.getSize().x - 1)
                renderer.toggleIf(row * 2 + 2, col * 2 + 1, sf::Color::White, sf::Color(242, 94, 94));
        }
    }
}

// true is vertical, false is horizontal
bool MazeSolver::pickOrientation(int width, int height) {
    return (width <= height) ? (width == height ? (irand(0, 100) > 50 ? true : false) : false) : true;
}

void MazeSolver::recursiveDivision(Maze& maze, sf::RenderWindow& window, Renderer& renderer, int delay, int row, int col, int width, int height, bool orientation) {
    if ((width <= 0 && orientation) || (height <= 0 && !orientation))
        return;

    if (orientation) {
        // vertical slice
        int sliceCol = irand(col, col + width - 1);
        int gapIndex = irand(row, row + height);
        for (int i = row; i < row + height + 1; i++) {
            if (i != gapIndex) 
                maze.toggleWall(i, sliceCol, Maze::Direction::Right, renderer, sf::Color::White, sf::Color::Black);
            renderer.toggleWall(i * 2 + 2, sliceCol * 2 + 2, sf::Color::Black);
        }

        if (width != 1) 
            update(window, renderer, delay);
        recursiveDivision(maze, window, renderer, delay, row, sliceCol + 1, width - (sliceCol - col + 1), height, pickOrientation(width - (sliceCol - col + 1), height));
        recursiveDivision(maze, window, renderer, delay, row, col, sliceCol - col, height, pickOrientation(sliceCol - col, height));
    }
    else {
        // horizontal slice
        int sliceRow = irand(row, row + height - 1);
        int gapIndex  = irand(col, col + width);
        for (int i = col; i < col + width + 1; i++) {
            if (i != gapIndex)
                maze.toggleWall(sliceRow, i, Maze::Direction::Down, renderer, sf::Color::White, sf::Color::Black);
            renderer.toggleWall(sliceRow * 2 + 2, i * 2 + 2, sf::Color::Black);
        }

        if (height != 1) 
            update(window, renderer, delay);
        recursiveDivision(maze, window, renderer, delay, sliceRow + 1, col, width, height - (sliceRow - row + 1), pickOrientation(width, height - (sliceRow - row + 1)));
        recursiveDivision(maze, window, renderer, delay, row, col, width, sliceRow - row, pickOrientation(width, sliceRow - row));
    }
}