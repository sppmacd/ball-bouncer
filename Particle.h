#pragma once

#include "Entity.h"

class Particle : public Entity
{
public:
    enum Type
    {
        Good,
        Bad,
        Collision
    };

    Particle(Game& game, Type type);

    static constexpr int LIFETIME = 30;

    virtual void update() override;
    virtual void render(sf::RenderTarget&) const override;
    virtual void setPosition(sf::Vector2f position) override { m_rectangle.setPosition(position); }

private:
    sf::Vector2f m_motion;
    sf::RectangleShape m_rectangle;
    int m_lifetime = LIFETIME;
    Type m_type = Good;
};
