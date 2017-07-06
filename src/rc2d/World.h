#ifndef RC2D_WORLD_H
#define RC2D_WORLD_H

#include "Line.h"
#include <vector>

namespace sf {
    class CircleShape;
}

/**
 * The world where raycasting takes place.
 */
class World {

public:

    /**
     * \brief (constructor) Creates a new World object.
     */
    World(sf::RenderWindow *mainWindow);

    /**
     * \brief (destructor) Destroys any allocated resources.
     */
    ~World();

    /**
     * \brief Updates the game world.
     */
    virtual void update(sf::Time deltaTime);

    /**
     * \brief Draws the game world.
     */
    virtual void draw(sf::RenderWindow *window);

private:

    /// Helper functions.
    void updatePlayer(float dt);
    Intersection raycastEnvironment(const sf::Vector2f& origin, const sf::Vector2f& direction);

    /// Instance variables.
    sf::CircleShape *m_player;
    sf::Vector2f m_vel;
    sf::RenderWindow *m_window;
    std::vector<Line> m_lines;

    /// Static constants:
    static const float EPSILON;

};

#endif // RC2D_WORLD_H
