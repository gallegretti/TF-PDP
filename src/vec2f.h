#pragma once
struct vec2f {
	vec2f() = default;
	vec2f(float x, float y) : x(x), y(y)
	{
	}

	inline vec2f operator+(vec2f rhs)
	{
		return vec2f(x + rhs.x, y + rhs.y);
	}

	inline vec2f operator-(vec2f rhs)
	{
		return vec2f(x - rhs.x, y - rhs.y);
	}

	inline static float squared_distance(vec2f lhs, vec2f rhs)
	{
		return pow((lhs.x - rhs.x), 2) + pow((rhs.y - rhs.y), 2);
	}
	inline float length() {
		return sqrt((x * x) + (y * y));
	}
	inline float squared_length() {
		return (x * x) + (y * y);
	}
	inline void normalize() {
		float factor = 1 / length();
		x *= factor;
		y *= factor;
	}
	float x;
	float y;
};
