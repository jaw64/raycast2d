#ifndef RC2D_GAME_H
#define RC2D_GAME_H
#include "World.h"
#include <SFML/Graphics.hpp>

/**
 * This is the class holding the entire "game".
 */
class Game {

public:

    /**
     * \brief (constructor) Creates a new Game object.
     */
    Game();

    /**
     * \brief (destructor) Destroys any allocated resources.
     */
    ~Game();

    /**
     * \brief Runs the game.
     * \return the game return code
     */
    virtual int run();

    /// Static constants:
    static const int WIDTH;
    static const int HEIGHT;
    static const int FRAMERATE;

private:
    /// Helper functions:
    virtual void update(sf::Time dt);
    virtual void draw();

    /// Instance variables:
    sf::RenderWindow m_window; // The main window.
    World m_world; // The game world.

};

#endif // RC2D_GAME_H
