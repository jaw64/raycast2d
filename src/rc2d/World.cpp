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
        l.draw(window, sf::Color::White);
    }

    // Get the direction of the mouse and player.
    sf::Vector2f pp = m_player->getPosition();
    sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(*window));
    sf::Vector2f mousedir = mp - pp;
    float mouseAngle = std::atan2(mousedir.y, mousedir.x);

    // Get the list of points to cast from the lines.
    std::vector<sf::Vector2f> points;
    // Get left angle:
    float darad = VIEW_ANGLE * M_PI / 360.0f;
    float leftAngle = mouseAngle - darad;
    float rightAngle = mouseAngle + darad;
    sf::Vector2f leftDir = sf::Vector2f(std::cos(leftAngle), std::sin(leftAngle));
    sf::Vector2f rightDir = sf::Vector2f(std::cos(rightAngle), std::sin(rightAngle));
    std::cout << VectorUtils::dot(VectorUtils::normalize(mousedir), VectorUtils::normalize(rightDir)) << std::endl;
    points.push_back(pp + leftDir);
    points.push_back(pp + rightDir);
    for (unsigned int i=0; i < m_lines.size(); i++) {
        std::vector<sf::Vector2f> p(4, sf::Vector2f(0.0f, 0.0f));
        sf::Vector2f p1 = m_lines[i].p1;
        sf::Vector2f p2 = m_lines[i].p2;
        p[0] = p1 + VectorUtils::normalize(p2 - p1) * EPSILON;
        p[1] = p2 + VectorUtils::normalize(p1 - p2) * EPSILON;
        p[2] = p1 + VectorUtils::normalize(p1 - p2) * EPSILON;
        p[3] = p2 + VectorUtils::normalize(p2 - p1) * EPSILON;
        for (unsigned int j=0; j < p.size(); j++) {
            sf::Vector2f toAdd = p[j];
            sf::Vector2f pointdir = toAdd - pp;
            float ata = std::atan2(pointdir.y, pointdir.x); // angle to add
            mousedir = VectorUtils::normalize(mousedir);
            pointdir = VectorUtils::normalize(pointdir);
            float angleBetween = std::acos(VectorUtils::dot(mousedir, pointdir));
            float vangle = VIEW_ANGLE * M_PI / 360.0f; // view-angle
            if (angleBetween > vangle) {
                continue;
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
    for (unsigned int i=0; i < points.size(); i++) {
        int index = i;
        //if (i == points.size()) index = 0;
        //if (points.size() == 0) break;
        sf::Vector2f point = points[index];
        sf::Vector2f dir = VectorUtils::normalize(point - pp);
        Intersection isect = raycastEnvironment(pp, dir);
        sf::Vector2f ip;
        if (isect.exists) {
            ip = isect.point;
        } else {
            ip = pp + dir * VIEW_DISTANCE;
        }
        vao.append(sf::Vertex(ip, lightColor));
    }
    window->draw(vao);

    // Draw the points in order.
    for (unsigned int i=0; i < points.size(); i++) {
        sf::Vector2f point = points[i];
        sf::CircleShape cs(5.0f);
        cs.setOrigin(5.0f, 5.0f);
        cs.setPosition(point);
        float value = static_cast<float>(i+1) / static_cast<float>(points.size());
        unsigned char val = static_cast<unsigned char>(value * 255.0f);
        sf::Color color(0x00, val, 0x00);
        cs.setFillColor(color);
        Line(point, pp).draw(window, color);
        window->draw(cs);
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
