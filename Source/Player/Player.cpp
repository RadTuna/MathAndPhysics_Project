
#include "Precompiled.h"
#include "Player.h"

void Player::SetMovementSpeed(const float NewSpeed)
{
    _MovementSpeed = NewSpeed;
}

float Player::GetMovementSpeed() const
{
    return _MovementSpeed;
}

void Player::SetInput(const float NewX, const float NewY)
{
    _CurrentInputX = NewX;
    _CurrentInputY = NewY;
}

void Player::Update(const float DeltaTime)
{
    Vector2 movementVector;
    movementVector.X = _CurrentInputX * _MovementSpeed * DeltaTime;
    movementVector.Y = _CurrentInputY * _MovementSpeed * DeltaTime;

    GetTransform().AddPosition(movementVector);
}
