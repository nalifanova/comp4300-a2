#ifndef VEC2_H
#define VEC2_H

struct Vec2
{
public:
    Vec2() = default;
    Vec2(float xIn, float yIn);

    bool operator ==(const Vec2& rhs) const;
    bool operator !=(const Vec2& rhs) const;

    Vec2 operator +(const Vec2& rhs) const;
    Vec2 operator -(const Vec2& rhs) const;
    Vec2 operator /(float val) const;
    Vec2 operator *(float val) const;

    bool operator +=(const Vec2& rhs) const;
    bool operator -=(const Vec2& rhs) const;
    bool operator *=(float val) const;
    bool operator /=(float val) const;

    [[nodiscard]] float dist(const Vec2& rhs) const;

    float x = 0;
    float y = 0;
};

#endif //VEC2_H
