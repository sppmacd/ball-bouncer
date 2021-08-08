#include "Resources.h"

bool Resources::load()
{
    if(!ballTexture.loadFromFile("res/ball.png"))
        return false;
    if(!goodBouncerTexture.loadFromFile("res/good-bouncer.png"))
        return false;
    if(!badBouncerTexture.loadFromFile("res/bad-bouncer.png"))
        return false;

    if(!font.loadFromFile("res/font.ttf"))
        return false;

    if(!bouncerSound.loadFromFile("res/bouncer.wav"))
        return false;
    if(!bouncerBadSound.loadFromFile("res/bouncer-bad.wav"))
        return false;
    if(!bouncerDeathSound.loadFromFile("res/bouncer-death.wav"))
        return false;
    if(!newBallSound.loadFromFile("res/new-ball.wav"))
        return false;
    if(!errorSound.loadFromFile("res/error.wav"))
        return false;

    return true;
}
