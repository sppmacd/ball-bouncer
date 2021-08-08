#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct Resources
{
    sf::Texture ballTexture;
    sf::Texture goodBouncerTexture;
    sf::Texture badBouncerTexture;
    sf::Font font;
    sf::SoundBuffer bouncerSound;
    sf::SoundBuffer bouncerBadSound;
    sf::SoundBuffer bouncerDeathSound;
    sf::SoundBuffer newBallSound;

    bool load();
};
