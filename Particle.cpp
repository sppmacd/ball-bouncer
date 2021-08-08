#include "Particle.h"

#include "Game.h"

Particle::Particle(Game& game, Type type)
: Entity(game)
, m_rectangle(sf::Vector2f(4, 4)), m_type(type)
{
    m_rectangle.setOrigin(2, 2);
    m_motion = sf::Vector2f((rand() % 256 - 128) / 32.f, m_type == Collision ? (rand() % 256) / -256.f : (rand() % 256 - 128) /-32.f);
}

void Particle::update()
{
    if(m_lifetime-- == 0)
    {
        setDead();
    }

    double lifetimeFactor = m_lifetime / static_cast<double>(LIFETIME);

    switch(m_type)
    {
        case Good: m_rectangle.setFillColor(sf::Color(0, 0, 200, 255 * lifetimeFactor)); break;
        case Bad: m_rectangle.setFillColor(sf::Color(200, 0, 0, 255 * lifetimeFactor)); break;
        case Collision: m_rectangle.setFillColor(sf::Color(20, 20, 20, 255 * lifetimeFactor)); break;
    }        

    m_rectangle.move(m_motion);
    m_motion /= m_type == Collision ? 1.1f : 1.05f;
    if(m_type == Collision)
        m_motion.y += Game::GRAVITY;
}

void Particle::render(sf::RenderTarget& target) const
{
    target.draw(m_rectangle);
}
