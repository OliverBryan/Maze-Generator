#ifndef RENDER_HPP
#define RENDER_HPP

class Maze;

#include <SFML/Graphics.hpp>
#include <vector>

class Renderer {
public:
    Renderer(Maze& maze, const sf::IntRect& viewport);

    void draw(sf::RenderWindow& window);
    void resize(Maze& maze, const sf::IntRect& viewport, sf::Color backgroundFill);
    void resize(Maze& maze, const sf::IntRect& viewport);
    void toggleWall(const int row, const int col, sf::Color fill);
    void toggleCell(const int row, const int col, sf::Color fill);
    void toggleIf(const int row, const int col, sf::Color fill, sf::Color condition);

private:
    std::vector<sf::RectangleShape> shapes;
};

#endif /* RENDER_HPP */