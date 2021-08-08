#include "PowerBox.h"

#include "Ball.h"

#include <iostream>

PowerBox::PowerBox(Game& game)
: Entity(game)
, m_rectangle(sf::Vector2f(16, 16))
{
    m_rectangle.setOrigin(8, 8);
    m_rectangle.setTexture(&m_game.resources().badBouncerTexture);
}

void PowerBox::render(sf::RenderTarget& target) const
{
    target.draw(m_rectangle);
}

void PowerBox::update()
{
    m_lifetime--;
    if(m_lifetime == 0)
    {
        m_game.spawnParticles(m_rectangle.getPosition(), 10, Particle::Good);
        setDead();
    }

    double lifetimeFactor = m_lifetime / 180.0;
    m_rectangle.setFillColor(sf::Color(255, 255, 255, lifetimeFactor * 255));
}

void PowerBox::onCollide(Entity& other)
{
    auto ball = dynamic_cast<Ball*>(&other);
    if(ball)
    {
        int power = rand() % 6;
        // This can actually give good things...
        switch(power)
        {
        case 0:
            ball->m_motion /= 2.f;
            m_game.addSplash("Motion x0.5", m_rectangle.getPosition());
            break;
        case 1:
            m_game.setBoxCount(m_game.boxCount() / 2);
            m_game.addSplash("-" + std::to_string(m_game.boxCount()) + " Bouncers", m_rectangle.getPosition());
            break;
        case 2:
            ball->m_motion /= 4.f;
            m_game.addSplash("Motion x0.25", m_rectangle.getPosition());
            break;
        case 3:
        case 4:
            m_game.setBoxCount(m_game.boxCount() + 20);
            m_game.addSplash("+20 Bouncers", m_rectangle.getPosition());
            break;
        case 5:
            ball->m_motion *= 2.f;
            m_game.addSplash("Motion x2", m_rectangle.getPosition());
            break;
        }
    }
    m_game.spawnParticles(m_rectangle.getPosition(), 10, Particle::Bad);
    setDead();
}

