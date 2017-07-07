#include "Game.h"

const int Game::WIDTH = 1280;
const int Game::HEIGHT = 720;
const int Game::FRAMERATE = 50;

Game::Game():
    m_world(&m_window)
{
    m_window.setFramerateLimit(FRAMERATE);
    m_window.setVerticalSyncEnabled(true);
    sf::ContextSettings css;
    css.antialiasingLevel = 4;
    m_window.create(sf::VideoMode(WIDTH, HEIGHT), "raycast2d",
                    sf::Style::Default, css);
}

Game::~Game()
{
}

int Game::run() {
    // Set a return code.
    int retCode = 0;

    // Start delta time clock.
    sf::Clock deltaClock;
    deltaClock.restart();

    // Start the game loop.
    while (m_window.isOpen()) {
        // Get elapsed time since last frame.
        sf::Time deltaTime = deltaClock.restart();

        // Get window events.
        sf::Event event;

        // Check for window closed.
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
        }

        try {
            // Update the game.
            update(deltaTime);

            // Draw the game.
            draw();
        }
        catch (int e) {
            retCode = e;
            break;
        }

    }
    return retCode;
}

void Game::update(sf::Time dt) {
    if (dt.asMilliseconds() > 0) {
        m_world.update(dt);
    }
}

void Game::draw() {
    // Clear the window.
    m_window.clear();
    m_world.draw(&m_window);
    m_window.display();
}
