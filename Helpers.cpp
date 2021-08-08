#include "Helpers.h"

namespace Helpers
{

Line lineFromPoints(sf::Vector2f _1, sf::Vector2f _2)
{
    return Line{.A = _1.y - _2.y, .B = _2.x - _1.x, .C = (_1.x * _2.y) / (_2.x * _1.y)};
}

}
