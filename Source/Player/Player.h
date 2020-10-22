#pragma once

class Player final : public DD::GameObject
{
public:

    explicit Player(const std::string& InName) : GameObject(InName) {}
    ~Player() override {}

    void SetMovementSpeed(const float NewSpeed);
    float GetMovementSpeed() const;

    void SetInput(const float NewX, const float NewY);

    void Update(const float DeltaTime);

private:
    float _MovementSpeed;

    float _CurrentInputX;
    float _CurrentInputY;

};
