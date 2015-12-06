#ifndef VEC2_H
#define VEC2_H

#include <cmath>

class Vec2 {
public:
	Vec2() :x(0), y(0){}
	Vec2(float xy) :x(xy), y(xy){}
	Vec2(float xx, float yy) :x(xx), y(yy){}

	float x, y;

	inline void set(const Vec2& v) { x = v.x; y = v.y; }
	inline void set(float xx, float yy) { x = xx; y = yy; }

	bool equals(const Vec2& v2, float epsilon) const;

	Vec2 rotate(float degrees) const;
	float dot(const Vec2& v) const;
	// TODO: Decide how to implement cross product
	// http://stackoverflow.com/questions/243945/calculating-a-2d-vectors-cross-product
	// Vec2 cross(const Vec2& v) const;

	Vec2 lerpTo( const Vec2& dest, float t );

	inline float lengthSquared() const { return x*x + y*y; }
	inline float length() const { return std::sqrt(x*x + y*y); }
	
	inline Vec2 normal() const { return Vec2(*this / length()); }
	inline Vec2& normalize() { set(normal()); return *this; }

	// stanadard vector arithmetic operators
	inline Vec2 operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
	inline Vec2 operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
	inline Vec2 operator * (const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
	inline Vec2 operator / (const Vec2& v) const { return Vec2(x / v.x, y / v.y); }

	// scalar arithmetic
	inline Vec2 operator * (float s) const { return Vec2(x * s, y * s); }
	inline Vec2 operator / (float s) const { return Vec2(x / s, y / s); }

	// assignment operators
	inline Vec2 operator += (const Vec2& v) { x += v.x; y += v.y; return *this; }
	inline Vec2 operator -= (const Vec2& v) { x -= v.x; y -= v.y; return *this; }
	inline Vec2 operator *= (float s) { x *= s; y *= s; return *this; }
	inline Vec2 operator /= (float s) { x /= s; y /= s; return *this; }
};

#endif
