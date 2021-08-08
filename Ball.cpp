#include "Ball.h"

#include "Helpers.h"

#include <iostream>
#include <math.h>

Ball::Ball(Game& game)
: Entity(game)
{
    m_sprite.setTexture(m_game.resources().ballTexture);
    m_sprite.setPosition(0, 0);
    m_sprite.setScale(2, 2);
    m_sprite.setOrigin(8, 8);
    m_motion.x = 3;
}

void Ball::render(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

void Ball::update()
{
    // Gravity
    m_motion.y += Game::GRAVITY;

    // Apply move + collisions
    auto position = m_sprite.getPosition();
    auto ballPositionAfterMove = position + m_motion;
    if(std::abs(ballPositionAfterMove.x) > MAP_BOUNDS)
    {
        // Bounce
        m_motion.x = -m_motion.x * BOUNCINESS;

        // Friction
        if(std::abs(m_motion.y) + FRICTION > MIN_NONZERO_VELOCITY)
            m_motion.y += m_motion.y < 0 ? FRICTION : -FRICTION;
        else
            m_motion.y = 0;

        // Reset offset
        ballPositionAfterMove.y = position.y + m_motion.y;
        ballPositionAfterMove.x = ballPositionAfterMove.x < 0 ? -MAP_BOUNDS : MAP_BOUNDS;

        // Rotation
        m_angularVelocity = m_angularVelocity / 4 - m_motion.y / 6.28 * 180 / RADIUS;

        // Dark particles
        m_game.spawnParticles({ballPositionAfterMove.x < 0 ? -MAP_BOUNDS-RADIUS : MAP_BOUNDS+RADIUS, position.y}, 10, Particle::Collision);
    }
    if(std::abs(ballPositionAfterMove.y) > MAP_BOUNDS)
    {
        // Bounce
        if(ballPositionAfterMove.y > MAP_BOUNDS)
            m_motion.y = -m_motion.y * BOUNCINESS_FLOOR;
        else
            m_motion.y = -m_motion.y * BOUNCINESS;

        // Friction
        if(std::abs(m_motion.x) + FRICTION > MIN_NONZERO_VELOCITY)
            m_motion.x += m_motion.x < 0 ? FRICTION : -FRICTION;
        else
            m_motion.x = 0;

        // Reset offset
        ballPositionAfterMove.x = position.x + m_motion.x;
        ballPositionAfterMove.y = ballPositionAfterMove.y < 0 ? -MAP_BOUNDS : MAP_BOUNDS;

        // Rotation
        m_angularVelocity = m_angularVelocity / 4 + m_motion.x / 6.28 * 180 / RADIUS;
    }

    for(auto& entity: m_game.entities())
    {
        if(entity.get() != this)
        {
            if(entity->boundingBox().intersects(boundingBox()))
            {
                entity->onCollide(*this);
            }
            // TODO: Add ball bouncing sometime
            /*// TODO: Avoid this dynamic_cast
            auto ball = dynamic_cast<Ball*>(entity.get());
            if(ball)
            {
                float dstx = m_sprite.getPosition().x - ball->m_sprite.getPosition().x;
                float dsty = m_sprite.getPosition().y - ball->m_sprite.getPosition().y;
                if(dstx * dstx + dsty * dsty < RADIUS * RADIUS * 4)
                {
                    // Useful constants
                    const float tx = m_sprite.getPosition().x;
                    const float ty = m_sprite.getPosition().y;
                    const float bx = ball->m_sprite.getPosition().x;
                    const float by = ball->m_sprite.getPosition().y;

                    // Ray of ball
                    const auto ballRay = Helpers::lineFromPoints( {tx, ty},
                                                            {tx + m_motion.x, ty + m_motion.y});


                    // Collision point equation
                    const double cl_a = (ballRay.A + 1) / ballRay.A;
                    const double cl_b = 2 * ballRay.B * ballRay.C + 2 * bx * ballRay.B - 2 * by;
                    const double cl_c = bx * bx + by * by - RADIUS * RADIUS + (ballRay.B + 2 * bx * ballRay.C + ballRay.C * ballRay.C) / ballRay.A;
                    const double cl_sqdelta = std::sqrt(cl_b*cl_b - 4*cl_a*cl_c);
                    const double cl_x1 = (-cl_b - cl_sqdelta) / (2 * cl_a);
                    const double cl_x2 = (-cl_b + cl_sqdelta) / (2 * cl_a);
                    const double cl_y1 = (-ballRay.B * cl_x1 - ballRay.C)/ballRay.A;
                    const double cl_y2 = (-ballRay.B * cl_x2 - ballRay.C)/ballRay.A;

                    // Take the point that is nearer to ball
                    const double dst_cl1x = cl_x1 - tx;
                    const double dst_cl1y = cl_y1 - ty;
                    const double dst_cl2x = cl_x2 - tx;
                    const double dst_cl2y = cl_y2 - ty;

                    // Find out equation of normal
                    float normal_x, normal_y;
                    if(dst_cl1x*dst_cl1x + dst_cl1y*dst_cl1y < dst_cl2x*dst_cl2x + dst_cl2y*dst_cl2y)
                    {
                        normal_x = cl_x1;
                        normal_y = cl_y1;
                    }
                    else
                    {
                        normal_x = cl_x2;
                        normal_y = cl_y2;
                    }
                    const auto normal = Helpers::lineFromPoints({normal_x, normal_y}, {bx, by});

                    // Calculate mirrored vector
                    const double um_x = normal_x - m_motion.x;
                    const double um_y = normal_y - m_motion.y;

                    const double um_dst = std::abs(normal.A * um_x + normal.B * um_y + normal.C) / std::sqrt(normal.A * normal.A + normal.B * normal.B);
                    const double alpha = std::atan2(normal.A, -normal.B);
                    const double m_alpha = std::atan2(m_motion.x, -m_motion.y);
                    const double m_beta = 3.14/2 - m_alpha;
                    const double mtangle = 3.14/2 - (alpha - m_alpha); // Motion -> tangent angle
                    const double ds_angle = 3.14 - mtangle - m_beta; // Distance vector angle
                    const double ds_x = std::cos(ds_angle) * um_dst;
                    const double ds_y = std::sin(ds_angle) * um_dst;
                    const float mmotion_x = um_x + ds_x * 2 - normal_x;
                    const float mmotion_y = um_y + ds_y * 2 - normal_y;

                    m_motion = {mmotion_x, mmotion_y};
                }
            }*/
        }
    }

    m_sprite.setPosition(ballPositionAfterMove);
    m_sprite.rotate(m_angularVelocity);

    if(speed() < 2)
    {
        int otherColors = m_game.tickCount() / 10 % 2 == 0 ? 255 : 0;
        m_sprite.setColor(sf::Color(255, otherColors, otherColors));
    }
    else
        m_sprite.setColor(sf::Color::White);
}
