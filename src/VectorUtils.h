#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <SFML/System.hpp>
#include <cmath>

class VectorUtils {

public:

    /**
     * \brief Computes the dot product between two vectors.
     * \param v1 the first vector
     * \param v2 the second vector
     * \return the dot product between the two specified vectors
     */
    static float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    /**
     * \brief Normalizes 2D vectors.
     * \param vec the vector to normalize
     * \return the normalized vector
     */
    static sf::Vector2f normalize(const sf::Vector2f& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        return sf::Vector2f(v.x / len, v.y / len);
    }

    /**
     * \brief Gets the distance between two vectors.
     * \param v1 the first point
     * \param v2 the second point
     * \return the distance between the two vectors
     */
    static float distance(const sf::Vector2f& v1, const sf::Vector2f& v2) {
        float xcom = v1.x - v2.x;
        float ycom = v1.y - v2.y;
        return std::sqrt(xcom * xcom + ycom * ycom);
    }

    /**
     * \brief Gets the squared distance between two vectors (faster than distance).
     * \param v1 the first point
     * \param v2 the second point
     * \return the squared distance between the two vectors
     */
    static float distance2(const sf::Vector2f& v1, const sf::Vector2f& v2) {
        float xcom = v1.x - v2.x;
        float ycom = v1.y - v2.y;
        return xcom * xcom + ycom * ycom;
    }

    /**
     * \brief Gets a test vector's orientation with respect to a reference vector.
     * \param r the reference vector
     * \param t the test vector
     * \return 1 if t is clockwise from r, -1 if anticlockwise, and 0 if parallel
     */
    static int side(const sf::Vector2f& r, const sf::Vector2f& t) {
        float v1yv2x = r.y * t.x;
        float v1xv2y = r.x * t.y;
        if (v1yv2x > v1xv2y) {
            return 1;
        }
        else if (v1yv2x < v1xv2y) {
            return -1;
        }
        else {
            return 0;
        }
    }
};

#endif // VECTORUTILS_H
