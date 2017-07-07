#include "Line.h"
#include "../VectorUtils.h"

Line::Line()
{
}

Line::Line(const sf::Vector2f& endp1, const sf::Vector2f& endp2):
    p1(endp1),
    p2(endp2)
{
}

Line::Line(float x1, float y1, float x2, float y2):
    p1(x1, y1),
    p2(x2, y2)
{
}

Line::~Line()
{
}

void Line::draw(sf::RenderWindow *window, const sf::Color& color) const {
    sf::Vertex line[] = { sf::Vertex(p1, color), sf::Vertex(p2, color) };
    window->draw(line, 2, sf::Lines);
}

Intersection Line::castRay(const sf::Vector2f& origin, const sf::Vector2f& direction) const {
    Intersection isect;
    float px = origin.x; float py = origin.y;
    float dx = direction.x; float dy = direction.y;
    float ax = p1.x; float ay = p1.y;
    float bx = p2.x; float by = p2.y;
    float bxmax = bx - ax; float bymay = by - ay;
    float denominator = (dy * (bxmax)) - (dx * (bymay));
    // Lines are parallel.
    if (denominator == 0.0f) {
        return isect;
    }
    // Intersection does exist on infinite.
    float numerator = (dx * (ay - py)) - (dy * (ax - px));
    float s = numerator / denominator;
    // Intersection exists on line segment.
    if (s >= 0.0f && s <= 1.0f) {
        float t = -1.0f;
        if (dx != 0.0f) {
            t = (ax - px + (bxmax * s)) / dx;
        }
        else if (dy != 0.0f) {
            t = (ay - py + (bymay * s)) / dy;
        }
        // Intersection is in front of ray.
        if (t >= 0.0f) {
            // Calculate intersection point.
            isect.point = p1 + ((p2 - p1) * s);
            isect.exists = true;

            // Calculate normal.
            sf::Vector2f norm1(-bymay, bxmax);
            sf::Vector2f norm2(bymay, -bxmax);
            sf::Vector2f tp = p1 + norm1; // test point
            float testSide = (tp.x - ax) * bymay - (tp.y - ay) * bxmax;
            float side = (px - ax) * bymay - (py - ay) * bxmax;
            if (side == 0.0f) {
                isect.normal = VectorUtils::normalize(p2 - p1);
            } else {
                bool sameSide = side * testSide > 0.0f;
                if (sameSide) {
                    isect.normal = VectorUtils::normalize(norm1);
                } else {
                    isect.normal = VectorUtils::normalize(norm2);
                }
            }
        }
    }
    return isect;
}
