#include "World.h"
#include "../VectorUtils.h"
#include <SFML/Graphics.hpp>

const float World::EPSILON = 0.001f;

World::World(sf::RenderWindow *mainWindow):
    m_player(new sf::CircleShape(20.0f)),
    m_vel(0.0f, 0.0f),
    m_window(mainWindow)
{
    // Create the player.
    m_player->setFillColor(sf::Color::Red);
    m_player->setOrigin(m_player->getRadius(), m_player->getRadius());
    m_player->setPosition(640.0f, 360.0f);

    // Create the lines.
    // Borders:
    //m_lines.push_back(Line(0.0f, 0.0f, 1280.0f, 0.0f));
    //m_lines.push_back(Line(1280.0f, 720.0f, 1280.0f, 0.0f));
    //m_lines.push_back(Line(0.0f, 720.0f, 1280.0f, 720.0f));
    //m_lines.push_back(Line(0.0f, 0.0f, 0.0f, 720.0f));

    // Walls:
    m_lines.push_back(Line(300.0f, 250.0f, 500.0f, 100.0f));
    m_lines.push_back(Line(700.0f, 300.0f, 700.0f, 500.0f));
    m_lines.push_back(Line(850.0f, 350.0f, 950.0f, 450.0f));
    m_lines.push_back(Line(450.0f, 600.0f, 400.0f, 500.0f));
    m_lines.push_back(Line(500.0f, 450.0f, 650.0f, 550.0f));
    m_lines.push_back(Line(700.0f, 200.0f, 850.0f, 300.0f));
}

World::~World()
{
    delete m_player;
}

void World::update(sf::Time deltaTime) {
    // Update the player.
    updatePlayer(deltaTime.asSeconds());
}

void World::draw(sf::RenderWindow *window) {
    // Draw the environment.
    for (unsigned int i=0; i < m_lines.size(); i++) {
        Line l = m_lines[i];
        l.draw(window);
    }

    // Draws green squares for where to cast rays.
    for (unsigned int i=0; i < m_lines.size(); i++) {
        Line cl = m_lines[i]; // the current line being raycasted
        sf::Vector2f p1 = cl.p1;
        sf::Vector2f p2 = cl.p2;
        p1 = p1 + (VectorUtils::normalize(p1 - p2) * EPSILON);
        p2 = p2 + (VectorUtils::normalize(p2 - p1) * EPSILON);
        sf::CircleShape cs(4.0f, 4);
        cs.setOrigin(4.0f, 4.0f);
        cs.setFillColor(sf::Color::Green);
        cs.setPosition(p1);
        window->draw(cs);
        cs.setPosition(p2);
        window->draw(cs);
    }

    // Get player position and mouse position.
    sf::Vector2f origin = m_player->getPosition();
    sf::Vector2f mp(sf::Mouse::getPosition(*window));

    // Raycast the entire environment.
    // Get the directions to send rays.
    for (unsigned int i=0; i < m_lines.size(); i++) {
        Line il = m_lines[i];
        sf::Vector2f p1 = il.p1;
        sf::Vector2f p2 = il.p2;
        p1 = p1 + (VectorUtils::normalize(p1 - p2) * EPSILON);
        p2 = p2 + (VectorUtils::normalize(p2 - p1) * EPSILON);
        sf::Vector2f dir1 = VectorUtils::normalize(p1 - origin); // cast ray in this direction
        sf::Vector2f dir2 = VectorUtils::normalize(p2 - origin); // cast ray in this direction

        // Actually cast the rays.
        Intersection isect1 = raycastEnvironment(origin, dir1);
        Intersection isect2 = raycastEnvironment(origin, dir2);

        // Graphics stuff.
        sf::CircleShape cs(4.0f);
        cs.setOrigin(4.0f, 4.0f);
        cs.setFillColor(sf::Color::Red);

        // Draw the intersections.
        if (isect1.exists) {
            cs.setPosition(isect1.point);
            Line(origin, isect1.point).draw(window);
            window->draw(cs);
        } else {
            Line(origin, origin + dir1 * 2000.0f).draw(window);
        }
        if (isect2.exists) {
            cs.setPosition(isect2.point);
            Line(origin, isect2.point).draw(window);
            window->draw(cs);
        } else {
            Line(origin, origin + dir2 * 2000.0f).draw(window);
        }
    }

    // Draw intersection points.
    sf::Vector2f dir = VectorUtils::normalize(mp - origin);
    Intersection isect = raycastEnvironment(origin, dir);
    if (isect.exists) {
        sf::CircleShape cs(4.0f);
        cs.setOrigin(4.0f, 4.0f);
        cs.setFillColor(sf::Color::Yellow);
        cs.setPosition(isect.point);
        Line(origin, isect.point).draw(window);
        window->draw(cs);
    } else {
        Line(origin, origin + dir * 2000.0f).draw(window);
    }

    // Draw the player.
    window->draw(*m_player);
}

void World::updatePlayer(float dt) {
    sf::Vector2f goal(0.0f, 0.0f);
    const float PLAYER_SPEED = 200.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        goal.y -= PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        goal.x -= PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        goal.y += PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        goal.x += PLAYER_SPEED;
    }
    if (goal != sf::Vector2f(0.0f, 0.0f)) {
        goal = VectorUtils::normalize(goal) * PLAYER_SPEED;
    }
    float smoothing = 0.02f;
    sf::Vector2f acc = smoothing * (goal - m_vel);
    m_vel = m_vel + acc;
    m_player->setPosition(m_player->getPosition() + m_vel * dt);
}

Intersection World::raycastEnvironment(const sf::Vector2f& origin, const sf::Vector2f& direction) {
    Intersection best;
    for (unsigned int i=0; i < m_lines.size(); i++) {
        Line cl = m_lines[i]; // the current line being raycasted
        // Cast to point 1 first.
        Intersection isect = cl.castRay(origin, direction);
        if (isect.exists) {
            float bestd = VectorUtils::distance2(origin, best.point);
            float currd = VectorUtils::distance2(origin, isect.point);
            if (!best.exists || currd < bestd) {
                best = isect;
            }
        }
    }
    return best;
}
