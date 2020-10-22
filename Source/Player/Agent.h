#pragma once

class Agent final : public DD::GameObject
{
public:

    explicit Agent(const std::string& InName)
        : GameObject(InName)
        , _DetectHalfFOV(60.f)
        , _DetectDistance(50.f)
        , _MovementSpeed(50.f)
        , _MovementLimitHeight(100.f)
        , _MovementLimitWidth(100.f)
        , _PatrolPosition(GetTransform().GetPosition())
        , _TargetGameObject(nullptr) {}
    ~Agent() override {}

    void SetDetectProperties(const float NewDetectHalfFOV, const float NewDetectDistance);
    float GetDetectHalfFov() const;
    float GetDetectDistance() const;

    bool IsDetect(const GameObject& InGameObject) const;
    void EnterDetect(const GameObject& InGameObject);
    void ExitDetect();
    bool IsDetected() const;

    void SetMovementSpeed(const float NewMovementSpeed);
    float GetMovementSpeed() const;

    void SetMovementLimits(const float NewLimitHeight, const float NewLimitWidth);
    float GetMovementLimitHeight() const;
    float GetMovementLimitWidth() const;

    void Update(const float DeltaTime);

private:
    static float GetRotationFromDirection(const Vector2& dir);

private:
    float _DetectHalfFOV;
    float _DetectDistance;
    float _FOVLimit;

    float _MovementSpeed;
    float _MovementLimitHeight;
    float _MovementLimitWidth;
    Vector2 _PatrolPosition;
    const GameObject* _TargetGameObject;

};
