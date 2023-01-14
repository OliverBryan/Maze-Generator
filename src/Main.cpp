#include <SFML/Graphics.hpp>
#include "Maze.hpp"
#include "Render.hpp"
#include "MazeSolver.hpp"
#include "Interface.hpp"

#include <string>
#include <future>
#include <chrono>
#include <thread>

// SOURCES:
// http://www.astrolog.org/labyrnth/algrithm.htm
// http://www.neocomputer.org/projects/eller.html

// trick to make waiting for future more pleasant
template<typename T>
bool isReady(std::future<T> const& f){ 
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
}

// start solver on another thread
void getHandle(MazeSolver& solver, Maze& maze, sf::RenderWindow& window, Renderer& renderer, RunInfo info, std::future<void>& handle, sf::IntRect viewport) {
    switch (info.algo) {
    case MazeSolver::Algorithm::RecursiveBacktrack:
        handle = std::async(std::launch::async, [&solver, &maze, &window, &renderer, info]() {
            start(&MazeSolver::recursiveBacktrack, solver, maze, window, renderer, info.delay, 0, 0, Maze::Direction::None);
        });
        break;
    case MazeSolver::Algorithm::GrowingTree:
        handle = std::async(std::launch::async, [&solver, &maze, &window, &renderer, info]() {
            start(&MazeSolver::growingTree, solver, maze, window, renderer, info.delay);
        });
        break;
    case MazeSolver::Algorithm::Ellers:
        handle = std::async(std::launch::async, [&solver, &maze, &window, &renderer, info]() {
            start(&MazeSolver::ellers, solver, maze, window, renderer, info.delay);
        });
        break;
    case MazeSolver::Algorithm::RecursiveDivision:
        handle = std::async(std::launch::async, [&solver, &maze, &window, &renderer, info, viewport]() {
            maze.removeWalls();
            renderer.resize(maze, viewport, sf::Color::White);
            start(&MazeSolver::recursiveDivision, solver, maze, window, renderer, info.delay, 0, 0, maze.getSize().x - 1, maze.getSize().y - 1, solver.irand(0, 100) > 50 ? true : false);
        });
        break;
    default:
        //unreachable
        exit(-1);
    }
}

int main() {
    // attempt to load any automated runs
    auto runs = ui::loadRunsFromFile("run.dat");
    
    // if there are preloaded runs, run them, otherwise get user input
    RunInfo info;
    if (runs.size() > 0) {
        info = runs[0];
        runs.erase(runs.begin());
    }
    else info = ui::configureRun(RunInfo {MazeSolver::Algorithm::RecursiveBacktrack, sf::Vector2u(10, 10), 10});

    // create maze and maze solver
    Maze maze(info.mazeSize.x, info.mazeSize.y);
    std::future<RunInfo> inputHandle;
    MazeSolver solver;

    // open window
    sf::ContextSettings cs;
    cs.antialiasingLevel = 0;
    sf::RenderWindow window(sf::VideoMode(600, 600), "Final Project", sf::Style::Default, cs);

    // intialize renderer
    sf::IntRect viewport(sf::Vector2i(0, 0), sf::Vector2i(window.getSize().x, window.getSize().y));
    Renderer renderer(maze, viewport);

    // unactivate the window so opengl doesn't get mad when the window gets drawn to from antoher thread
    auto b = window.setActive(false);

    // start the animation
    std::future<void> handle;
    getHandle(solver, maze, window, renderer, info, handle, viewport); 

    bool done = false;
    // input loop
    while (window.isOpen()) {
        // poll for events (doing this on the main thread allows the animation to only draw when it needs to instead of every frame)
        sf::Event e;
        while (window.pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }

        // animation is already done and cleanup has occured
        if (done) {
            // display the generated maze
            window.clear(sf::Color::White);
            renderer.draw(window);
            window.display();
            
            // wait for input from user input thread
            if (isReady(inputHandle)) {
                // get user input
                info = inputHandle.get();

                // re unactivate the window (see above)
                b = window.setActive(false);

                // update the maze and renderer for the new run info
                maze.resize(info.mazeSize.x, info.mazeSize.y);
                renderer.resize(maze, viewport);

                // start the animation again
                getHandle(solver, maze, window, renderer, info, handle, viewport);
                done = false;
            }
        }

        // animation is done
        if (isReady(handle) && !done) {
            done = true;

            // if there are more automated runs then run them
            if (runs.size() > 0) {
                inputHandle = std::async(std::launch::async, [&runs]() {
                    RunInfo info = runs[0];
                    runs.erase(runs.begin());
                    return info;
                });
            }
            
            // otherwise get user input on another thread so that the window will stay open
            else inputHandle = std::async(std::launch::async, &ui::configureRun, info);
            continue;
        }
    }

    return 0;    
}