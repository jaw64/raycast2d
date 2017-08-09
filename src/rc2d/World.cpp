#include "World.h"
#include "Game.h"
#include "../VectorUtils.h"
#include <iostream>

const float World::EPSILON = 0.08f;
const float World::VIEW_DISTANCE = 450.0f;
const float World::VIEW_DISTANCE_2 = World::VIEW_DISTANCE * World::VIEW_DISTANCE;
const float World::VIEW_ANGLE = 90.0f;

World::World(sf::RenderWindow *mainWindow):
    m_player(new sf::CircleShape(20.0f)),
    m_floor(new sf::Sprite()),
    m_floortex(new sf::Texture()),
    m_shader(new sf::Shader()),
    m_rt(new sf::RenderTexture()),
    m_vel(0.0f, 0.0f),
    m_window(mainWindow)
{
    // Load shader:
    if (!m_shader->loadFromFile("src/shaders/mask.frag", sf::Shader::Fragment)) {
        std::cerr << "[ERROR]: Could not load shader." << std::endl;
    }

    // Load render texture for sprite:
    if (!m_rt->create(Game::WIDTH, Game::HEIGHT)) {
        std::cerr << "[ERROR]: Could not create render texture." << std::endl;
    }
    m_rt->setSmooth(true);

    // Create the floor sprite.
    if (!m_floortex->loadFromFile("res/floor.png")) {
        std::cerr << "[ERROR]: Could not load floor.png." << std::endl;
    }
    m_floor->setTexture(*m_floortex);

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
    //srand(time(NULL));
    //for (int i = 0; i < 10; i++) {
    //    float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1280.0f;
    //    float x2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1280.0f;
    //    float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 720.0f;
    //    float y2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 720.0f;
    //    m_lines.push_back(Line(x1, y1, x2, y2));
    //}
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
    delete m_floortex;
    delete m_floor;
    delete m_shader;
    delete m_rt;
}

void World::update(sf::Time deltaTime) {
    // Update the player.
    updatePlayer(deltaTime.asSeconds());
}

void World::draw(sf::RenderWindow *window) {
    // Draw the environment.
    window->draw(*m_floor);
    for (unsigned int i=0; i < m_lines.size(); i++) {
        Line l = m_lines[i];
        //l.draw(window, sf::Color::White);
    }

    // Get the direction of the mouse and player.
    sf::Vector2f pp = m_player->getPosition();
    sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(*window));
    sf::Vector2f mousedir = mp - pp;
    float mouseAngle = std::atan2(mousedir.y, mousedir.x);

    // Get the list of points to cast from the lines.
    std::vector<sf::Vector2f> points;

    // Cast many rays in the direction the player is facing:
    const float darad = VIEW_ANGLE * M_PI / 360.0f;
    const float leftAngle = mouseAngle - darad;
    const float rightAngle = mouseAngle + darad;
    const int slices = 180;
    const float da = VIEW_ANGLE * M_PI / (180.0f * static_cast<float>(slices));
    for (float currAngle = leftAngle; currAngle < rightAngle; currAngle += da) {
        sf::Vector2f dir = sf::Vector2f(std::cos(currAngle), std::sin(currAngle));
        points.push_back(pp + dir);
        if (currAngle + da >= rightAngle) {
            dir = sf::Vector2f(std::cos(rightAngle), std::sin(rightAngle));
            points.push_back(pp + dir);
        }
    }

    // Cast rays toward line endpoints.
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
            mousedir = VectorUtils::normalize(mousedir);
            pointdir = VectorUtils::normalize(pointdir);
            float angleBetween = std::acos(VectorUtils::dot(mousedir, pointdir));
            float vangle = VIEW_ANGLE * M_PI / 360.0f; // view-angle
            if (angleBetween > vangle) {
                continue;
            }
            for (unsigned int k=0; k <= points.size(); k++) {
                if (k >= points.size()) {
                    points.push_back(toAdd);
                    break;
                }
                sf::Vector2f toComp = points[k];
                sf::Vector2f testdir = toComp - pp;
                if (VectorUtils::side(testdir, pointdir) > 0) {
                    points.insert(points.begin() + k, toAdd);
                    break;
                }
            }
        }
    }

    // Add points to VAO in rotational order.
    sf::VertexArray vao;
    vao.setPrimitiveType(sf::TriangleFan);
    sf::Color lightColor(0xff, 0xff, 0xff);
    vao.append(sf::Vertex(pp, lightColor));
    for (unsigned int i=0; i < points.size(); i++) {
        int index = i;
        if (i == points.size()) index = 0;
        if (points.size() == 0) break;
        sf::Vector2f point = points[index];
        sf::Vector2f dir = VectorUtils::normalize(point - pp);
        Intersection isect = raycastEnvironment(pp, dir);
        sf::Vector2f ip;
        if (isect.exists) {
            if (VectorUtils::distance2(pp, isect.point) > VIEW_DISTANCE_2) {
                ip = pp + dir * VIEW_DISTANCE;
            } else {
                ip = isect.point;
            }
        } else {
            ip = pp + dir * VIEW_DISTANCE;
        }
        float pct = (VIEW_DISTANCE - VectorUtils::distance(pp, ip)) / VIEW_DISTANCE;
        if (pct < 0.0f) pct = 0.0f;
        if (pct > 1.0f) pct = 1.0f;
        unsigned char val = static_cast<unsigned char>(pct * 255.0f);
        lightColor = sf::Color(val, val, val);
        vao.append(sf::Vertex(ip, lightColor));
    }

    // Draw points to separate render texture.
    m_rt->clear(sf::Color::Black);
    m_rt->draw(vao);
    m_rt->display();

    // Render texture to window as a sprite through the masking shader.
    m_shader->setUniform("maskTexture", m_rt->getTexture());
    m_shader->setUniform("invisibleColor", sf::Vector3f(0.0f, 0.0f, 0.0f));
    m_shader->setUniform("visibleColor", sf::Vector3f(1.0f, 1.0f, 0.5f));
    window->draw(sf::Sprite(m_rt->getTexture()), m_shader);
    m_rt->clear(sf::Color(0xff, 0x00, 0x00)); // [TODO]: Figure out why this is required.

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
