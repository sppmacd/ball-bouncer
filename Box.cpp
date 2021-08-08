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
        float fac = std::min(1.5f, 1.1f * WARMUP_TICKS / (m_game.gameTickCount() + 1));
        ball->m_motion.x += ball->m_motion.x > 0 ? fac : -fac;
        ball->m_motion.y += ball->m_motion.y > 0 ? fac : -fac;
        ball->m_motion.y -= std::min(7.f, std::max(1.f, WARMUP_TICKS * 8 / (std::min(WARMUP_TICKS, m_game.gameTickCount() + 1.f))));
    }
    m_game.spawnParticles(m_rectangle.getPosition(), 10, Particle::Good);
    setDead();
    m_game.playSound(m_game.resources().bouncerSound);
}
