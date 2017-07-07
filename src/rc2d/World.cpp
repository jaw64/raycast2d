#include "World.h"
#include "../VectorUtils.h"
#include <iostream>

const float World::EPSILON = 0.1f;
const float World::VIEW_DISTANCE = 2000.0f;
const float World::VIEW_DISTANCE_2 = World::VIEW_DISTANCE * World::VIEW_DISTANCE;
const float World::VIEW_ANGLE = 90.0f;

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
    m_lines.push_back(Line(0.0f, 0.0f, 1280.0f, 0.0f));
    m_lines.push_back(Line(1280.0f, 720.0f, 1280.0f, 0.0f));
    m_lines.push_back(Line(0.0f, 720.0f, 1280.0f, 720.0f));
    m_lines.push_back(Line(0.0f, 0.0f, 0.0f, 720.0f));

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
        //l.draw(window, sf::Color::Black);
    }

    // Get the direction of the mouse and player.
    sf::Vector2f pp = m_player->getPosition();
    sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(*window));
    sf::Vector2f mousedir = mp - pp;
    float mouseAngle = std::atan2(mousedir.y, mousedir.x);
    mousedir = VectorUtils::normalize(mousedir);

    // Get the list of points to cast from the lines.
    std::vector<sf::Vector2f> points;
    for (unsigned int i=0; i < m_lines.size(); i++) {
        std::vector<sf::Vector2f> p(4, sf::Vector2f(0.0f, 0.0f));
        sf::Vector2f p1 = m_lines[i].p1;
        sf::Vector2f p2 = m_lines[i].p2;
        p[0] = p1 + VectorUtils::normalize(p2 - p1) * EPSILON;
        p[1] = p2 + VectorUtils::normalize(p1 - p2) * EPSILON;
        p[2] = p1 + VectorUtils::normalize(p1 - p2) * EPSILON;
        p[3] = p2 + VectorUtils::normalize(p2 - p1) * EPSILON;
        const int psize = static_cast<int>(p.size());
        for (int j=-1; j <= psize; j++) {
            sf::Vector2f toAdd;
            if (j < 0 || j >= psize) {
                // [TODO]: Do the angle VIEW_ANGLE/2 degrees less.
                float vangle = VIEW_ANGLE * M_PI / 360.0f;
                float angle = j < 0 ? mouseAngle - vangle : mouseAngle + vangle;
                float dx = std::cos(angle);
                float dy = std::sin(angle);
                toAdd = sf::Vector2f(pp + sf::Vector2f(dx, dy));
                continue; // [TODO]: REMOVE
            } else {
                toAdd = p[j];
            }
            float ata = std::atan2(toAdd.y - pp.y, toAdd.x - pp.x); // angle to add
            if (!angleInView(mouseAngle, ata)) {
                //continue; [TODO]: REMOVE
            }
            if (points.empty()) {
                points.push_back(toAdd);
            } else {
                for (unsigned int k=0; k <= points.size(); k++) {
                    if (k == points.size()) {
                        points.push_back(toAdd);
                        break;
                    }
                    sf::Vector2f toComp = points[k];
                    float atc = std::atan2(toComp.y - pp.y, toComp.x - pp.x); // angle to compare
                    if (ata <= atc) {
                        points.insert(points.begin() + k, toAdd);
                        break;
                    }
                }
            }
        }
    }

    // Draw the points in order:
    sf::VertexArray vao;
    vao.setPrimitiveType(sf::TriangleFan);
    sf::Color lightColor(0x66, 0x66, 0x66);
    vao.append(sf::Vertex(pp, lightColor));
    for (unsigned int i=0; i <= points.size(); i++) {
        int index = i;
        if (i == points.size()) index = 0;
        if (points.size() == 0) break;
        sf::Vector2f point = points[index];
        sf::Vector2f dir = VectorUtils::normalize(point - pp);
        Intersection isect = raycastEnvironment(pp, dir);
        sf::Vector2f ip;
        if (isect.exists && VectorUtils::distance2(isect.point, pp) <= VIEW_DISTANCE_2) {
            ip = isect.point;
        } else {
            ip = pp + dir * VIEW_DISTANCE;
        }
        vao.append(sf::Vertex(ip, lightColor));
    }
    window->draw(vao);

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

bool World::angleInView(float mouseAngle, float testAngle) const {
    mouseAngle += M_PI;
    testAngle += M_PI;
    float hangle = VIEW_ANGLE * M_PI / 360.0f; // half-angle
    float langle = mouseAngle - hangle; // low-angle
    float tangle = mouseAngle + hangle; // high-angle
    const float PI_2 = M_PI * 2.0f;
    if (langle < 0.0f) {
        float dangle = -langle;
        tangle += dangle;
        langle = 0.0f;
    }
    if (tangle > PI_2) {
        float dangle = tangle - PI_2;
        langle -= dangle;
        tangle = PI_2;
    }
    return testAngle <= tangle && testAngle >= langle;
}
