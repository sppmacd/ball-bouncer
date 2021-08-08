#pragma once

#include "Entity.h"
#include "Game.h"

#include <cmath>

class Ball : public Entity
{
public:
    Ball(Game&);

    // TODO: Make it surface-dependent
    constexpr static float BOUNCINESS = 0.95;
    constexpr static float FRICTION = 0.01;

    constexpr static float MIN_NONZERO_VELOCITY = FRICTION * 2;
    constexpr static float RADIUS = 15;
    constexpr static float MAP_BOUNDS = Game::MAP_BOUNDS - RADIUS * 2;

    virtual void render(sf::RenderTarget&) const override;
    virtual void update() override;

    virtual void setPosition(sf::Vector2f pos) override { m_sprite.setPosition(pos); }
    virtual sf::FloatRect boundingBox() const override { return m_sprite.getGlobalBounds(); }

    float speed() const { return std::sqrt(m_motion.x*m_motion.x + m_motion.y*m_motion.y); }

    void addMotion(float x, float y) { m_motion += {x, y}; }

private:
    friend class Box;
    friend class PowerBox;

    sf::Sprite m_sprite;
    sf::Vector2f m_motion;
    float m_angularVelocity = 0;
};
