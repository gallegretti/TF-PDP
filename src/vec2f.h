#pragma once
struct vec2f {
	vec2f() = default;
	vec2f(float x, float y) : x(x), y(y)
	{
	}
	float length() {
		return sqrt((x * x) + (y * y));
	}
	float squared_length() {
		return (x * x) + (y * y);
	}
	float x;
	float y;
};
