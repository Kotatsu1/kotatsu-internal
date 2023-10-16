#pragma once
#include <stdint.h>
#include <vector>

extern struct ScreenCoordinates
{
	float X;
	float Y;
};

extern struct espData
{
	float feetX;
	float feetY;
	float headX;
	float headY;
	int health;
};

extern struct Matrix
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
};

extern struct Vector3
{
	float x;
	float y;
	float z;
};


class ESP
{
public:
	std::vector<espData> GetESPData();
};