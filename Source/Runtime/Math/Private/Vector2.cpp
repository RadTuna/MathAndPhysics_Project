
#include "Precompiled.h"
using namespace CK;

const Vector2 Vector2::UnitX(1.f, 0.f);
const Vector2 Vector2::UnitY(0.f, 1.f);
const Vector2 Vector2::Zero(0.f, 0.f);
const Vector2 Vector2::One(1.f, 1.f);

Vector2 Vector2::Interpolation(const Vector2& current, const Vector2& target, float speed, float deltaTime)
{
	const float x = Math::InterpolateFloat(current.X, target.X, speed, deltaTime);
	const float y = Math::InterpolateFloat(current.Y, target.Y, speed, deltaTime);

	return Vector2(x, y);
}

std::string Vector2::ToString() const
{
	char result[64];
	std::snprintf(result, sizeof(result), "(%.3f, %.3f)", X, Y);
	return result;
}
