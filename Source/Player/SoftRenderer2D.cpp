
#include "Precompiled.h"
#include "SoftRenderer.h"

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
    // �׸��� ����
    LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

    // ���� ���� ���� �׸���
    ScreenPoint screenHalfSize = mScreenSize.GetHalf();

    for (int x = screenHalfSize.X; x <= mScreenSize.X; x += mGrid2DUnit)
    {
        mRSI->DrawFullVerticalLine(x, gridColor);
        if (x > screenHalfSize.X)
        {
            mRSI->DrawFullVerticalLine(2 * screenHalfSize.X - x, gridColor);
        }
    }

    for (int y = screenHalfSize.Y; y <= mScreenSize.Y; y += mGrid2DUnit)
    {
        mRSI->DrawFullHorizontalLine(y, gridColor);
        if (y > screenHalfSize.Y)
        {
            mRSI->DrawFullHorizontalLine(2 * screenHalfSize.Y - y, gridColor);
        }
    }

    // ���� �� �׸���
    mRSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
    mRSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
    // ���� �������� ����ϴ� ����
    static float moveSpeed = 100.f;
    static float scaleSpeed = 100.f;
    static float rotateSpeed = 2.f;
    static float transformSpeed = 0.3f;

    mCurrentTime += InDeltaSeconds;

    // ���� ��⿡�� �Է� ������ ��������
    InputManager input = mGameEngine.GetInputManager();
    const Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
    const float deltaRotation = std::sinf(mCurrentTime * transformSpeed) * -rotateSpeed * InDeltaSeconds;
    const float deltaScale = std::sinf(mCurrentTime * transformSpeed) * scaleSpeed * InDeltaSeconds;

    mCurrentPosition += deltaPosition;
    mCurrentRotation += deltaRotation;
    mCurrentScale += deltaScale;

    mCurrentColor = input.IsSpacePressed() ? LinearColor::Red : LinearColor::Blue;
}


// ������ ����
void SoftRenderer::Render2D()
{
    // ���� �׸���
    //DrawGrid2D();

    static float drawStep = 1.f;
    static float drawAngle = 0.1f;
    static float circleIncrease = 0.5f;
    static float circleIncreaseMultiplier = 1.003f;
    static float particleLength = 50.f;
    static float particleInterval = 10.f;

    float currentAngle = 0.f;
    float currentParticleLength = 0.f;
    float currentParticleInterval = 0.f;
    float currentDrawAngle = drawAngle;

    HSVColor hsvColor(0.f, 1.0f, 1.0f);
    for (float radius = 0.f; radius < mCurrentScale; radius += circleIncrease)
    {
        const float currentDrawLimit = currentAngle + currentDrawAngle;
        const float weightedDrawStep = drawStep / (radius + 0.0001f);
        for (float angle = currentAngle; angle < currentDrawLimit; angle += weightedDrawStep)
        {
            if (currentParticleLength < particleLength)
            {
                const float x = radius * std::cosf(angle + mCurrentRotation);
                const float y = radius * std::sinf(angle + mCurrentRotation);

                hsvColor.H = angle * Math::InvPI * 0.5f;
                hsvColor.V = currentParticleLength / particleLength;
                mRSI->DrawPoint(Vector2(x, y), hsvColor.ToLinearColor());
                currentParticleLength += radius * weightedDrawStep;
            }
            else if (currentParticleInterval > particleInterval)
            {
                currentParticleLength = 0.f;
                currentParticleInterval = 0.f;
            }
            else
            {
                currentParticleInterval += radius * weightedDrawStep;
            }
        }

        currentAngle = currentDrawLimit;
        currentDrawAngle /= circleIncreaseMultiplier;
    }

    // ���� ��ġ�� ȭ�鿡 ���
    mRSI->PushStatisticText(CIRCLE_ORIGIN_TEXT + mCurrentPosition.ToString());
}

