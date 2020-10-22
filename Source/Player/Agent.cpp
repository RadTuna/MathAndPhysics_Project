
#include "Precompiled.h"
#include "Agent.h"

#include <random>

void Agent::SetDetectProperties(const float NewDetectHalfFOV, const float NewDetectDistance)
{
    _DetectHalfFOV = NewDetectHalfFOV;
    _DetectDistance = NewDetectDistance;

    float sin;
    float cos;
    Math::GetSinCos(sin, cos, _DetectHalfFOV);
    _FOVLimit = cos;
}

float Agent::GetDetectHalfFov() const
{
    return _DetectHalfFOV;
}

float Agent::GetDetectDistance() const
{
    return _DetectDistance;
}

bool Agent::IsDetect(const GameObject& InGameObject) const
{
    // 우선 시야각 내에 존재하는 지 검사.
    const Vector2 toTargetVector = InGameObject.GetTransform().GetPosition() - GetTransform().GetPosition();
    const Vector2 normalizeTargetVector = toTargetVector.Normalize();

    float sin;
    float cos;
    Math::GetSinCos(sin, cos, GetTransform().GetRotation());
    const Vector2 forwardVector(cos, sin);

    const float currentDot = normalizeTargetVector.Dot(forwardVector);
    if (currentDot < _FOVLimit)
    {
        return false;
    }

    // 거리 내에 존재하는 지 검사.
    if (_DetectDistance < toTargetVector.Size())
    {
        return false;
    }

    return true;
}

void Agent::EnterDetect(const GameObject& InGameObject)
{
    _TargetGameObject = &InGameObject;
}

void Agent::ExitDetect()
{
    _TargetGameObject = nullptr;
}

bool Agent::IsDetected() const
{
    return _TargetGameObject != nullptr;
}

void Agent::SetMovementSpeed(const float NewMovementSpeed)
{
    _MovementSpeed = NewMovementSpeed;
}

float Agent::GetMovementSpeed() const
{
    return _MovementSpeed;
}

void Agent::SetMovementLimits(const float NewLimitHeight, const float NewLimitWidth)
{
    _MovementLimitHeight = NewLimitHeight;
    _MovementLimitWidth = NewLimitWidth;
}

float Agent::GetMovementLimitHeight() const
{
    return _MovementLimitHeight;
}

float Agent::GetMovementLimitWidth() const
{
    return _MovementLimitWidth;
}

void Agent::Update(const float DeltaTime)
{
    const float HalfHeight = _MovementLimitHeight * 0.5f;
    const float HalfWidth = _MovementLimitWidth * 0.5f;

    std::random_device randDev;
    std::uniform_real_distribution<float> distX(-HalfWidth, HalfWidth);
    std::uniform_real_distribution<float> distY(-HalfHeight, HalfHeight);

    if (_TargetGameObject == nullptr) // 순찰
    {
        const Vector2 distanceVector = _PatrolPosition - GetTransform().GetPosition();
        if (distanceVector.SizeSquared() < 1e-06f) // 도착
        {
            _PatrolPosition.X = distX(randDev);
            _PatrolPosition.Y = distY(randDev);
        }
        else // 계속 추격
        {
            const Vector2 normalizeDistanceVector = distanceVector.Normalize();
            const float distance = distanceVector.Size();
            if (distance < _MovementSpeed * DeltaTime)
            {
                GetTransform().SetPosition(_PatrolPosition);
            }
            else
            {
                GetTransform().AddPosition(normalizeDistanceVector * (_MovementSpeed * DeltaTime));
            }

            GetTransform().SetRotation(Math::Rad2Deg(GetRotationFromDirection(normalizeDistanceVector)));
        }
    }
    else // 추격
    {
        if (!IsDetect(*_TargetGameObject))
        {
            ExitDetect();
            return;
        }

        const Vector2 distanceVector = _TargetGameObject->GetTransform().GetPosition() - GetTransform().GetPosition();
        const Vector2 normalizeDistanceVector = distanceVector.Normalize();
        const float distance = distanceVector.Size();
        if (distance < _MovementSpeed * DeltaTime)
        {
            GetTransform().SetPosition(_TargetGameObject->GetTransform().GetPosition());
        }
        else
        {
            GetTransform().AddPosition(normalizeDistanceVector * (_MovementSpeed * DeltaTime));
        }

        GetTransform().SetRotation(Math::Rad2Deg(GetRotationFromDirection(normalizeDistanceVector)));
    }
}

float Agent::GetRotationFromDirection(const Vector2& dir)
{
    const float dotX = Vector2::UnitX.Dot(dir);
    const float dotY = Vector2::UnitY.Dot(dir);

    float angle = std::acosf(dotX);
    if (dotY < 0.f)
    {
        angle = Math::TwoPI - angle;
    }

    return angle;


}
