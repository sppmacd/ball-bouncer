#include "Game.h"

#include "Ball.h"
#include "Box.h"
#include "Particle.h"
#include "PowerBox.h"

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

Game::~Game()
{
    // Save highscore
    m_highScore = m_score;
    std::ofstream file("playerdata.txt");
    file << m_highScore;
}

bool Game::load()
{
    if(!m_resources.load())
        return false;

    // Sounds
    if(m_backgroundMusic.openFromFile("res/music.wav"))
    {
        m_backgroundMusic.setLoop(true);
        m_backgroundMusic.play();
    }

    // Game
    m_gameOverTick = -2;
    m_balls.push_back(spawnEntity<Ball>());
    m_bouncerForce = 4;

    std::ifstream file("playerdata.txt");
    if(!file.good())
    {
        std::cout << "No playerdata.txt file found" << std::endl;
        m_highScore = 0;
    }
    // Don't care about errors.
    file >> m_highScore;
    
    // Window
    m_window.create(sf::VideoMode::getDesktopMode(), "Ball Bouncer");
    m_window.setFramerateLimit(60);

    // Force sending resize event on Windows
    sf::Event resizeEvent;
    resizeEvent.type = sf::Event::Resized;
    resizeEvent.size.width = m_window.getSize().x;
    resizeEvent.size.height = m_window.getSize().y;
    handleEvent(resizeEvent);

    return true;
}

void Game::run()
{
    while(m_running)
    {
        // events
        sf::Event event;
        while(m_window.pollEvent(event))
        {
            handleEvent(event);
        }

        tick();
        m_tickCount++;
        m_window.clear(sf::Color(20, 20, 20));
        render(m_window);
        m_window.display();
    }
}

void Game::handleEvent(sf::Event const& event)
{
    switch(event.type)
    {
        case sf::Event::Closed:
            m_running = false;
            break;
        case sf::Event::Resized:
        {
            float aspect = static_cast<double>(event.size.width) / event.size.height;
            float x = aspect > 1 ? SCREEN_SIZE * aspect : SCREEN_SIZE;
            float y = aspect > 1 ? SCREEN_SIZE : SCREEN_SIZE / aspect;
            m_window.setView({{0, 0}, {x, y}});
        } break;
        case sf::Event::MouseButtonPressed:
        {
            auto pos = m_window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if(std::abs(pos.x) < MAP_BOUNDS && std::abs(pos.y) < MAP_BOUNDS)
            {
                if(m_boxCount > 0 || m_gameOverTick != -1)
                {
                    std::cout << m_boxCount << ":" << m_gameOverTick << std::endl;
                    if(m_boxCount > 0)
                        m_boxCount--;
                    auto box = spawnEntity<Box>();
                    box->setPosition(pos);
                    if(m_gameOverTick == -1)
                        addSplash("Bouncers left: " + std::to_string(m_boxCount), pos);
                }
                else
                {
                    addSplash("No bouncers", pos);
                    playSound(m_resources.errorSound);
                }
            }
        } break;
        case sf::Event::KeyPressed:
        {
            switch(event.key.code)
            {
                case sf::Keyboard::Space:
                    if(m_gameOverTick != -1)
                        setup();
                    break;
                case sf::Keyboard::Escape:
                    if(m_gameOverTick != -2)
                        m_gameOverTick = -2;
                    else
                        m_running = false;
                    break;
                default:
                    break;
            }
        } break;
        default:
            break;
    }
}

void Game::setup()
{
    m_score = 0;
    m_gameOverTick = -1;
    m_gameTickCount = 0;
    m_entities.clear();
    m_boxCount = 15;
    m_balls.clear();
    m_balls.push_back(spawnEntity<Ball>());
    m_currentTip = 0;
    m_bouncerForce = 4;
}

void Game::tick()
{
    for(auto& entity: m_entities)
    {
        entity->update();
    }

    // Cleanup dead entities
    {
        auto remove = std::remove_if(m_entities.begin(), m_entities.end(), [](auto& entity) {
            return entity->isDead();
        });
        if(remove != m_entities.end())
            m_entities.erase(remove, m_entities.end());
    }

    // Cleanup stopped sounds
    {
        for(auto it = m_sounds.begin(); it != m_sounds.end();)
        {
            auto thisIt = it++;
            if(thisIt->getStatus() == sf::Sound::Stopped)
            {
                m_sounds.erase(thisIt);
            }
        }
    }

    if(m_gameOverTick == -1)
    {
        if(m_bouncerForce > 7)
            m_bouncerForce -= 0.0016;
        if(m_tickCount % 60 == 0)
        {
            m_score++;
            if(m_score % 2 == 0)
                m_boxCount += 3;
            if(m_score > m_highScore && m_highScore != 0)
                m_highScore = m_score;

            if(m_score % 15 == 0 && m_score > 0)
            {
                spawnMultipleParticles(10, 20, Particle::Good);
                playSound(m_resources.newBallSound);
                m_balls.push_back(spawnEntity<Ball>());
                for(auto& ball: m_balls)
                    ball->addMotion(0, -12);

                // In 15 seconds (900 ticks) bouncer force changes by 1.44. This should not make
                // force greater than it was before.
                m_bouncerForce += 1.3;
            }
        }
        for(auto& ball: m_balls)
        {
            if(ball->speed() < 0.3)
            {
                gameOver();
                break;
            }
        }
        if(rand() % 500 == 0)
        {
            float rx = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
            float ry = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
            auto box = spawnEntity<PowerBox>();
            box->setPosition({rx, ry});
        }
        m_gameTickCount++;
    }
    else if(m_gameOverTick == -2)
    {
        if(m_tickCount % TIP_TIME == 0)
        {
            static const sf::String TIPS[] = {
                "Click to place bouncers",
                "Bouncers add speed to ball",
                "A new ball spawns every 15 seconds",
                "Don't let any ball stop!",
                "Red boxes may contain a surprise!",
            };
            m_tip = TIPS[m_currentTip];
            m_currentTip++;
            if(m_currentTip >= 5)
                m_currentTip = 0;
        }
    }

    if(m_gameOverTick > 0)
    {
        float rx = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
        float ry = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
        spawnParticles({rx, ry}, 20, Particle::Bad);
        m_gameOverTick--;
    }
}

void Game::gameOver()
{
    m_gameOverTick = 120;
    m_gameTickCount = 0;
    m_bouncerForce = 4;
    m_highScore = m_score;
    std::ofstream file("playerdata.txt");
    file << m_highScore;
}

sf::Color operator*(sf::Color const& in, float m)
{
    return {static_cast<sf::Uint8>(in.r*m), static_cast<sf::Uint8>(in.g*m), static_cast<sf::Uint8>(in.b*m), static_cast<sf::Uint8>(in.a*m)};
}

void Game::render(sf::RenderTarget& target)
{
    sf::RectangleShape rs({(MAP_BOUNDS - Ball::RADIUS) * 2, (MAP_BOUNDS - Ball::RADIUS) * 2});
    rs.setOrigin(rs.getSize() / 2.f);
    rs.setTexture(&m_resources.backgroundTexture);
    target.draw(rs);

    for(auto& entity: m_entities)
    {
        entity->render(target);
    }

    // GUI
    if(m_gameOverTick == -1)
    {
        sf::Text scoreText(std::to_string(m_score), m_resources.font, 60);
        scoreText.setPosition(0, -MAP_BOUNDS + 40);
        scoreText.setOrigin(scoreText.getGlobalBounds().getSize().x / 2.f, 0);
        scoreText.setScale(0.5, 0.5);
        target.draw(scoreText);

        if(m_highScore > 0)
        {
            sf::Text highscoreText("(Highscore: " + std::to_string(m_highScore) + ")", m_resources.font, 30);
            highscoreText.setPosition(0, -MAP_BOUNDS + 80);
            highscoreText.setOrigin(highscoreText.getGlobalBounds().getSize().x / 2.f, 0);
            highscoreText.setScale(0.5, 0.5);
            target.draw(highscoreText);
        }

        sf::Text bouncersText("BOUNCERS: " + std::to_string(m_boxCount), m_resources.font, 30);
        bouncersText.setPosition(-MAP_BOUNDS + 25, -MAP_BOUNDS + 25);
        bouncersText.setScale(0.5, 0.5);
        target.draw(bouncersText);

        sf::Text ballsText("BALLS: " + std::to_string(m_balls.size()), m_resources.font, 30);
        ballsText.setPosition(MAP_BOUNDS - 25, -MAP_BOUNDS + 25);
        ballsText.setOrigin(ballsText.getLocalBounds().getSize().x, 0);
        ballsText.setScale(0.5, 0.5);
        target.draw(ballsText);
    }
    else
    {
        if((m_gameOverTick > 0 && m_gameOverTick / 15 % 2 == 0) || m_gameOverTick == 0)
        {
            sf::Text gameOverText("GAME OVER!", m_resources.font, 60);
            gameOverText.setScale(0.5, 0.5);
            gameOverText.setFillColor(sf::Color(230, 200, 0));
            gameOverText.setOrigin(gameOverText.getLocalBounds().getSize() / 2.f);
            gameOverText.setPosition(0, -100);
            target.draw(gameOverText);

            sf::Text lastScoreText("Your score: " + std::to_string(m_score) + " (" + std::to_string(m_balls.size()) + " balls)", m_resources.font, 40);
            lastScoreText.setScale(0.5, 0.5);
            lastScoreText.setFillColor(sf::Color(255, 120, 30));
            lastScoreText.setOrigin(lastScoreText.getLocalBounds().getSize() / 2.f);
            lastScoreText.setPosition(0, 20);
            target.draw(lastScoreText);
        }
        else if(m_gameOverTick == -2)
        {
            {
                sf::Text titleText("BALL BOUNCER", m_resources.font, 100);
                titleText.setStyle(sf::Text::Bold);
                float scale = (std::sin(m_tickCount / 30.f * 3.14f) + 64.f) / 128.f;
                titleText.setScale(scale, scale);
                titleText.setFillColor(sf::Color(230, 200, 0));
                titleText.setOrigin(titleText.getLocalBounds().getSize() / 2.f);
                titleText.setPosition(0, -100);
                target.draw(titleText);
            }
    
            {
                sf::Text tipText(m_tip, m_resources.font, 40);
                tipText.setScale(0.5, 0.5);
                int mTickCountModulo = m_tickCount % TIP_TIME;
                float alpha = mTickCountModulo < 15 ? mTickCountModulo / 15.f : (mTickCountModulo % TIP_TIME > (TIP_TIME-15) ? (TIP_TIME-mTickCountModulo) / 15.f : 1);
                tipText.setFillColor(sf::Color(255, 120, 30, alpha * 255));
                tipText.setOrigin(tipText.getLocalBounds().getSize() / 2.f);
                tipText.setPosition(0, 20);
                target.draw(tipText);
            }
    
            {
                sf::Text miniJamText("Made by Sppmacd for Mini Jam 86: Sports", m_resources.font, 20);
                miniJamText.setScale(0.5, 0.5);
                miniJamText.setFillColor(sf::Color(200, 200, 30));
                miniJamText.setOrigin(miniJamText.getLocalBounds().getSize() / 2.f);
                miniJamText.setPosition(0, MAP_BOUNDS - 50);
                target.draw(miniJamText);
            }
        }
    
        sf::Text gameOverTextSpace("< Press SPACE to start new game >", m_resources.font, 30);
        gameOverTextSpace.setScale(0.5, 0.5);
        gameOverTextSpace.setFillColor(sf::Color(200, 200, 30));
        gameOverTextSpace.setOrigin(gameOverTextSpace.getLocalBounds().getSize() / 2.f);
        gameOverTextSpace.setPosition(0, 60);
        target.draw(gameOverTextSpace);

        if(m_highScore > 0)
        {
            sf::Text highscoreText("Highscore: " + std::to_string(m_highScore), m_resources.font, 30);
            highscoreText.setPosition(0, -20);
            highscoreText.setOrigin(highscoreText.getGlobalBounds().getSize().x / 2.f, 0);
            highscoreText.setScale(0.5, 0.5);
            target.draw(highscoreText);
        }
    }

    for(auto& splash: m_splashes)
    {
        splash.lifetime--;
        splash.position.y -= 1;
        sf::Text splashText(splash.text, m_resources.font, 25);
        splashText.setScale(0.5, 0.5);
        splashText.setOrigin(splashText.getLocalBounds().getSize() / 2.f);
        splashText.setPosition(splash.position);
        splashText.setFillColor(sf::Color(255, 255, 255, splash.lifetime * 255.f / 180.f));
        target.draw(splashText);
    }

    auto splashRemove = std::remove_if(m_splashes.begin(), m_splashes.end(), [](auto& splash) {
        return splash.lifetime <= 0;
    });
    if(splashRemove != m_splashes.end())
        m_splashes.erase(splashRemove, m_splashes.end());
}

void Game::spawnParticles(sf::Vector2f position, size_t count, Particle::Type type)
{
    for(size_t s = 0; s < count; s++)
    {
        auto particle = spawnEntity<Particle>(type);
        particle->setPosition(position);
    }
}

void Game::spawnMultipleParticles(size_t centerCount, size_t count, Particle::Type type)
{
    for(size_t s = 0; s < centerCount; s++)
    {
        float rx = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
        float ry = rand() % static_cast<int>(MAP_BOUNDS * 2) - MAP_BOUNDS;
        spawnParticles({rx, ry}, count, type);
    }
}

void Game::playSound(sf::SoundBuffer const& buffer)
{
    m_sounds.emplace_back(buffer);
    auto& sound = m_sounds.back();
    sound.setVolume(50);
    sound.play();
}
