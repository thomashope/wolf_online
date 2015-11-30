#include "vec2.h"

Vec2 Vec2::rotate(float degrees) const {
	//angle is in degrees, positive clockwise
	//while (degrees > 360) degrees -= 360;
	//while (degrees < -360) degrees += 360;
	Vec2 result;
	if (degrees == 90 || degrees == -270) {
		// 90 degrees anit-clockwise
		result.x = y * -1;
		result.y = x;
	}
	else if (degrees == -90 || degrees == 270) {
		// 90 degrees clockwise
		result.x = y;
		result.y = x * -1;
	}
	else if (degrees == 180) {
		// turn that shit around
		result.x = x * -1;
		result.y = y * -1;
	}
	else {
		float radians = degrees * 3.14159265 / 180.0;
		// else apply rotation matrix
		result.x = x * cos(radians) + (y * sin(radians) * -1);
		result.y = x * sin(radians) + (y * cos(radians));
	}

	return result;
}

float Vec2::dot(const Vec2& v) const
{
	return x*v.x + y*v.y;
}

/*
Vec2 Vec2::cross(const Vec2& v) const
{

}

Vec2 Vec2::normal() const
{
	return Vec2(*this / length());
}

Vec2& Vec2::normalize()
{
	this->set(normal());
}*/
