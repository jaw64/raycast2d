#ifndef RC2D_LINE_H
#define RC2D_LINE_H

#include <SFML/System.hpp>
#include "Intersection.h"

namespace sf {
    class RenderWindow;
}

/**
 * Class that represents a line segment.
 */
class Line {

public:

    /**
     * \brief (constructor) Creates a new default Line object.
     */
    Line();

    /**
     * \brief (constructor) Creates a new Line object.
     */
    Line(sf::Vector2f endp1, sf::Vector2f endp2);

    /**
     * \brief (constructor) Creates a new Line object.
     */
    Line(float x1, float y1, float x2, float y2);

    /**
     * \brief (destructor) Destroys any allocated resources.
     */
    ~Line();

    /**
     * \brief Draws the line.
     */
    virtual void draw(sf::RenderWindow *window) const;

    /**
     * \brief Casts a ray to this line segment. Returns an intersection
     *        if one exists.
     * \param origin the origin point of the ray
     * \param direction the direction of the ray
     * \return a struct containing the point of intersection and the normal,
     *         if it exists
     */
    Intersection castRay(const sf::Vector2f& origin, const sf::Vector2f& direction) const;

    /// Public variables:
    sf::Vector2f p1;
    sf::Vector2f p2;

};

#endif // RC2D_LINE_H
