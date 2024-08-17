#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class CTransform
{
public:
    CTransform(const Vec2& p, const Vec2& v, float a):
        pos(p), velocity(v), angle(a) {}

    Vec2 pos = {0.0, 0.0};
    Vec2 velocity = {0.0, 0.0};
    float angle = 0;
};


class CShape
{
public:
    CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, const float thickness):
        circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }

    sf::CircleShape circle;
};

class CCollision
{
public:
    CCollision(float r):
        radius(r) {}

    float radius = 0;
};

class CScore
{
public:
    CScore(int s):
        score(s) {}

    int score = 0;
};

class CLifespan
{
public:
    CLifespan(int total):
        remaining(total), total(total) {}

    int remaining = 0; // amount of lifespan remaining on the entity
    int total = 0; // the total initial amount of lifespan
};

class CInput
{
public:
    CInput() = default;

    bool up = false;
    bool left = false;
    bool right = false;
    bool down = false;
    bool shoot = false;
};

#endif //COMPONENTS_H
