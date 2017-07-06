#ifndef RC2D_INTERSECTION_H
#define RC2D_INTERSECTION_H

struct Intersection {
    Intersection() : point(0.0f, 0.0f), normal(0.0f, 0.0f), exists(false) {}
    sf::Vector2f point;
    sf::Vector2f normal;
    bool exists;
};

#endif // RC2D_INTERSECTION_H
