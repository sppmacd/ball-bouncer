#pragma once

#include <SFML/System.hpp>

namespace Helpers
{

struct Line
{
    double A, B, C;
};

Line lineFromPoints(sf::Vector2f _1, sf::Vector2f _2);

}
