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

    bool operator +=(const Vec2& rhs);
    bool operator -=(const Vec2& rhs);
    bool operator *=(float val);
    bool operator /=(float val);

    [[nodiscard]] float dist(const Vec2& rhs) const;
    [[nodiscard]] float length() const;
    void normalize();

    float x = 0;
    float y = 0;
};

#endif //VEC2_H
