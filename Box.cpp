#include "Box.h"

#include "Ball.h"

#include <iostream>

Box::Box(Game& game)
: Entity(game)
, m_rectangle(sf::Vector2f(16, 16))
{
    m_rectangle.setOrigin(8, 8);
    m_rectangle.setTexture(&m_game.resources().goodBouncerTexture);
}

void Box::render(sf::RenderTarget& target) const
{
    target.draw(m_rectangle);
}

void Box::update()
{
    m_lifetime--;
    if(m_lifetime == 0)
    {
        m_game.spawnParticles(m_rectangle.getPosition(), 10, Particle::Bad);
        setDead();
        m_game.playSound(m_game.resources().bouncerDeathSound);
    }

    double lifetimeFactor = m_lifetime / 180.0;
    m_rectangle.setFillColor(sf::Color(255, 255, 255, lifetimeFactor * 255));
}

void Box::onCollide(Entity& other)
{
    auto ball = dynamic_cast<Ball*>(&other);
    if(ball)
    {
        float fac = m_game.bouncerForce();
        ball->m_motion.x += ball->m_motion.x > 0 ? fac : -fac;
        ball->m_motion.y -= fac;
    }
    m_game.spawnParticles(m_rectangle.getPosition(), 10, Particle::Good);
    setDead();
    m_game.playSound(m_game.resources().bouncerSound);
}
