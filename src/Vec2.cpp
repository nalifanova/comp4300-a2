#include "Vec2.h"

#include <math.h>

Vec2::Vec2(const float xIn, const float yIn):
    x(xIn), y(xIn) {}

bool Vec2::operator==(const Vec2& rhs) const
{
    // TODO
    return false;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    // TODO
    return false;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    // TODO
    return Vec2(0, 0);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    // TODO
    return Vec2(0, 0);
}

Vec2 Vec2::operator/(float val) const
{
    // TODO
    return Vec2(0, 0);
}

Vec2 Vec2::operator*(float val) const
{
    // TODO
    return Vec2(0, 0);
}

bool Vec2::operator+=(const Vec2& rhs) const
{
    // TODO
    return false;
}

bool Vec2::operator-=(const Vec2& rhs) const
{
    // TODO
    return false;
}

bool Vec2::operator*=(float val) const
{
    // TODO:
    return false;
}

bool Vec2::operator/=(float val) const
{
    // TODO
    return false;
}

float Vec2::dist(const Vec2& rhs) const
{
    return 0;
}
