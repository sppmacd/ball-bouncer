#pragma once

#include "Particle.h"
#include "Resources.h"

#include <list>
#include <memory>

class Ball;
class Entity;

class Game
{
public:
    static constexpr auto TIP_TIME = 180;

    ~Game();

    bool load();
    void run();

    void setup();

    size_t tickCount() const { return m_tickCount; }
    size_t gameTickCount() const { return m_gameTickCount; }

    void setBoxCount(size_t boxCount) { m_boxCount = boxCount; }
    size_t boxCount() const { return m_boxCount; }

    Resources const& resources() const { return m_resources; }

    template<class T, class... Args>
    std::shared_ptr<T> spawnEntity(Args&&... args)
    {
        auto entity = std::make_shared<T>(*this, std::forward<Args>(args)...);
        m_entities.push_back(entity);
        return entity;
    }

    void spawnParticles(sf::Vector2f position, size_t count, Particle::Type type);
    void spawnMultipleParticles(size_t centerCount, size_t count, Particle::Type type);

    std::list<std::shared_ptr<Entity>> const& entities() const { return m_entities; }
    
    constexpr static float MAP_BOUNDS = 250;
    constexpr static float SCREEN_SIZE = 500;
    constexpr static float GRAVITY = 0.5;

    void addSplash(sf::String text, sf::Vector2f position) { m_splashes.push_back({text, position}); }
    void playSound(sf::SoundBuffer const& buffer);
    double bouncerForce() const { return m_bouncerForce; }
    void gameOver();

private:
    void handleEvent(sf::Event const&);
    void tick();
    void render(sf::RenderTarget&);

    Resources m_resources;
    sf::RenderWindow m_window;
    std::list<std::shared_ptr<Entity>> m_entities;
    std::list<std::shared_ptr<Ball>> m_balls;

    struct Splash
    {
        sf::String text;
        sf::Vector2f position;
        int lifetime = 60;
    };

    std::list<Splash> m_splashes;
    std::list<sf::Sound> m_sounds;
    sf::String m_tip;
    size_t m_currentTip = 0;
    bool m_running = true;
    size_t m_tickCount = 0;
    size_t m_gameTickCount = 0;
    int m_score = 0;
    int m_highScore = 0;
    int m_gameOverTick = 30;
    size_t m_boxCount = 0;
    double m_bouncerForce = 0;
    sf::Music m_backgroundMusic;
};
