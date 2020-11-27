#pragma once

namespace CK
{

struct Transform2D
{
public:
	FORCEINLINE constexpr Transform2D() = default;
	FORCEINLINE constexpr Transform2D(const Vector2& InPosition) : Position(InPosition) { }
	FORCEINLINE constexpr Transform2D(const Vector2& InPosition, float InRotation) : Position(InPosition), Rotation(InRotation) { }
	FORCEINLINE constexpr Transform2D(const Vector2& InPosition, float InRotation, const Vector2& InScale) : Position(InPosition), Rotation(InRotation), Scale(InScale) { }

public: // 트랜스폼 설정함수
	constexpr void SetPosition(const Vector2& InPosition) { Position = InPosition; }
	constexpr void AddPosition(const Vector2& InDeltaPosition) { Position += InDeltaPosition; }
	void AddRotation(float InDegree) { Rotation += InDegree; Update(); }

	void SetRotation(float InDegree) { Rotation = InDegree; Update(); }
	constexpr void SetScale(const Vector2& InScale) { Scale = InScale; }

	FORCEINLINE constexpr Vector2 GetXAxis() const { return Right; }
	FORCEINLINE constexpr Vector2 GetYAxis() const { return Up; }
	constexpr Matrix3x3 GetMatrix() const;

	FORCEINLINE constexpr Vector2 GetPosition() const { return Position; }
	FORCEINLINE constexpr float GetRotation() const { return Rotation; }
	FORCEINLINE constexpr Vector2 GetScale() const { return Scale; }

	// 트랜스폼 변환
	FORCEINLINE Transform2D Inverse() const;
	FORCEINLINE Transform2D LocalToWorld(const Transform2D& InParentWorldTransform) const;
	FORCEINLINE Transform2D WorldToLocal(const Transform2D& InParentWorldTransform) const;

	void Update()
	{
		ClampAngle();
		float sin = 0.f, cos = 0.f;
		Math::GetSinCos(sin, cos, Rotation);
		Right = Vector2(cos, sin);
		Up = Vector2(-sin, cos);
	}

private:
	void ClampAngle()
	{
		Rotation = Math::FMod(Rotation, 360.f);
		if (Rotation < 0.f)
		{
			Rotation += 360.f;
		}
	}

private: // 트랜스폼에 관련된 변수
	Vector2 Position;
	float Rotation = 0.f;
	Vector2 Scale = Vector2::One;

	Vector2 Up = Vector2::UnitY;
	Vector2 Right = Vector2::UnitX;
};

FORCEINLINE constexpr Matrix3x3 Transform2D::GetMatrix() const
{
	return Matrix3x3(
		Vector3(GetXAxis() * Scale.X, false),
		Vector3(GetYAxis() * Scale.Y, false),
		Vector3(Position, true)
	);
}

FORCEINLINE Transform2D Transform2D::Inverse() const
{
	// 로컬 정보만 남기기 위한 트랜스폼 ( 역행렬 )
	Vector2 reciprocalScale = Vector2::Zero;
	if (!Math::EqualsInTolerance(Scale.X, 0.f)) reciprocalScale.X = 1.f / Scale.X;
	if (!Math::EqualsInTolerance(Scale.Y, 0.f)) reciprocalScale.Y = 1.f / Scale.Y;

	const Vector2 InversePosition = Vector2(
		GetXAxis().Dot(-Position) * reciprocalScale.X,
		GetYAxis().Dot(-Position) * reciprocalScale.Y);
	const Transform2D result(InversePosition, -Rotation, reciprocalScale);
	return result;
}


FORCEINLINE Transform2D Transform2D::LocalToWorld(const Transform2D& InParentWorldTransform) const
{
	const Vector2 NewPosition = InParentWorldTransform.GetMatrix() * Position;

	// 현재 트랜스폼 정보가 로컬인 경우
	Transform2D result;
    result.SetPosition(NewPosition);
	result.SetRotation(Rotation + InParentWorldTransform.Rotation);
	result.SetScale(Scale * InParentWorldTransform.Scale);

	return result;
}

FORCEINLINE Transform2D Transform2D::WorldToLocal(const Transform2D& InParentWorldTransform) const
{
	const Transform2D InverseTransform = InParentWorldTransform.Inverse();
	const Vector2 NewPosition(
		InParentWorldTransform.GetXAxis().Dot(Position) * InverseTransform.Scale.X,
		InParentWorldTransform.GetYAxis().Dot(Position) * InverseTransform.Scale.Y);

	// 현재 트랜스폼 정보가 월드인 경우
	Transform2D result;
	result.SetPosition(NewPosition + InverseTransform.Position);
	result.SetRotation(Rotation + InverseTransform.Rotation);
	result.SetScale(Scale * InverseTransform.Scale);

	return result;
}

}
