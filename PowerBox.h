#pragma once

#include "Entity.h"

class PowerBox : public Entity
{
public:
    PowerBox(Game& game);

    virtual void render(sf::RenderTarget& target) const override;
    virtual void update() override;

    virtual void setPosition(sf::Vector2f pos) override { m_rectangle.setPosition(pos); }
    virtual sf::FloatRect boundingBox() const override { return m_rectangle.getGlobalBounds(); }

    virtual void onCollide(Entity& other) override;

private:
    sf::RectangleShape m_rectangle;
    int m_lifetime = 180;
};

