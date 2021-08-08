#pragma once

#include <SFML/Graphics.hpp>

class Game;

class Entity
{
public:
    Entity(Game& game)
    : m_game(game) {}

    virtual void render(sf::RenderTarget&) const = 0;
    virtual void update() {}

    virtual void setPosition(sf::Vector2f) = 0;
    virtual sf::FloatRect boundingBox() const { return {-4, -4, 8, 8}; };
    virtual void onCollide(Entity& other) {}

    void setDead() { m_dead = true; }
    bool isDead() const { return m_dead; }

protected:
    Game& m_game;
    bool m_dead = false;
};
