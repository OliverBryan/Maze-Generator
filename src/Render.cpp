#include "Render.hpp"
#include "Maze.hpp"
#include <SFML/Graphics.hpp>

#include <utility>

constexpr int wallWidth = 8;

sf::RectangleShape getShape(bool cell, int dim, sf::Vector2i location, sf::Vector2f position, sf::Color backgroundFill) {
    sf::RectangleShape rs(sf::Vector2f(dim, dim));
    rs.setFillColor(cell ? backgroundFill : sf::Color::Black);
    rs.setPosition(position);

    if (location.x % 2 == 0 || location.y % 2 == 0) {
        float sizeX = (location.x % 2 == 0) ? dim / wallWidth : dim;
        float sizeY = (location.y % 2 == 0) ? dim / wallWidth : dim;

        rs.setSize(sf::Vector2f(sizeX, sizeY));
    }

    return rs;
}

Renderer::Renderer(Maze& maze, const sf::IntRect& viewport) {
    resize(maze, viewport);
}

void Renderer::resize(Maze& maze, const sf::IntRect& viewport, sf::Color backgroundFill) {
    shapes.clear();
    int dim = std::min((int) ((float) viewport.width * wallWidth) / ((wallWidth + 1) * (float) maze.cols + 1),
                       (int) ((float) viewport.height * wallWidth) / ((wallWidth + 1) * (float) maze.rows + 1));
    sf::Vector2f pos(viewport.left, viewport.top);
    for (int i = 0; i < maze.grid.size(); i++) {
        for (int j = 0; j < maze.grid[i].size(); j++) {
            auto rs = getShape(maze.grid[i][j], dim, sf::Vector2i(j, i), pos, backgroundFill);
            rs.setTextureRect(sf::IntRect(sf::Vector2i(i, j), sf::Vector2i(0, 0)));
            pos.x += rs.getSize().x;
            shapes.push_back(rs);
        }
        pos.y += (i % 2 == 0) ? dim / wallWidth : dim;
        pos.x = viewport.left;
    }

    
    int maxX = 0;
    int maxY = 0;
    for (sf::RectangleShape& rs : shapes) {
        if (rs.getPosition().x + rs.getSize().x > maxX)
            maxX = rs.getPosition().x + rs.getSize().x;
        if (rs.getPosition().y + rs.getSize().y > maxY)
            maxY = rs.getPosition().y + rs.getSize().y;
    }
    
    sf::Vector2f offset(viewport.left + viewport.width - maxX, viewport.top + viewport.height - maxY);
    for (sf::RectangleShape& rs : shapes)
        rs.setPosition(rs.getPosition() + offset / 2.0f);
}

void Renderer::resize(Maze& maze, const sf::IntRect& viewport) {
    resize(maze, viewport, sf::Color(204, 204, 204));
}

void Renderer::draw(sf::RenderWindow& window) {
    sf::VertexArray arr(sf::Triangles);
    for (sf::RectangleShape& rs : shapes) {
        for (int i = 0; i < 3; i++) {
            sf::Vector2f pos = rs.getPoint(i) + rs.getPosition();
            arr.append(sf::Vertex(pos, rs.getFillColor()));
        }
        for (int i = 0; i < 4; i++) {
            if (i == 1) continue;
            sf::Vector2f pos = rs.getPoint(i) + rs.getPosition();
            arr.append(sf::Vertex(pos, rs.getFillColor()));
        }
    }

    window.draw(arr);
}

void Renderer::toggleWall(const int row, const int col, sf::Color fill) {
    for (sf::RectangleShape& rs : shapes)
        if (rs.getTextureRect().left == row && rs.getTextureRect().top == col)
            rs.setFillColor(fill);
}

void Renderer::toggleCell(const int row, const int col, sf::Color fill) {
    for (sf::RectangleShape& rs : shapes)
        if (rs.getTextureRect().left == row && rs.getTextureRect().top == col)
            rs.setFillColor(fill);
}

void Renderer::toggleIf(const int row, const int col, sf::Color fill, sf::Color condition) {
    for (sf::RectangleShape& rs : shapes)
        if (rs.getTextureRect().left == row && rs.getTextureRect().top == col && rs.getFillColor() == condition)
            rs.setFillColor(fill);
}