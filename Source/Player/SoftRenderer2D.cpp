
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

void SoftRenderer::DrawBigPoint(const Vector2& targetPoint) const
{
	// ������ ���� �������� �����¿�� �� ���
	mRSI->DrawPoint(targetPoint, mCurrentColor);
	mRSI->DrawPoint(targetPoint + Vector2::UnitX, mCurrentColor);
	mRSI->DrawPoint(targetPoint - Vector2::UnitX, mCurrentColor);
	mRSI->DrawPoint(targetPoint + Vector2::UnitY, mCurrentColor);
	mRSI->DrawPoint(targetPoint - Vector2::UnitY, mCurrentColor);
}

void SoftRenderer::DrawCircle(float radius, float stepTheta) const
{
	for (float radian = 0.f; radian <= Math::TwoPI; radian += stepTheta)
	{
		Vector2 currentPoint(std::cosf(radian), std::sinf(radian));
		currentPoint = currentPoint * radius;
		currentPoint = currentPoint + mCurrentPosition;
		DrawBigPoint(currentPoint);
	}
}

void SoftRenderer::DrawRecursiveCircle(const Vector2& origin, float radius, float stepTheta, UINT32 numChild, UINT32 depth) const
{
	if (depth == 0)
	{
		return;
	}
	
	for (float radian = 0.f; radian <= Math::TwoPI; radian += stepTheta)
	{
		Vector2 currentPoint(std::cosf(radian), std::sinf(radian));
		currentPoint = currentPoint * radius;
		currentPoint = currentPoint + origin;
		//DrawBigPoint(currentPoint);
		mRSI->DrawPoint(currentPoint, mCurrentColor);
	}

	for (UINT32 index = 0; index < numChild; ++index)
	{
		float currentRadian = Math::TwoPI / numChild * index;
		currentRadian = Math::RadianClamp(currentRadian + mCircleRotationRadian);
		
		Vector2 childPoint(std::cosf(currentRadian), std::sinf(currentRadian));
		childPoint = childPoint * radius;
		childPoint = childPoint + origin;

		const float newRadius = radius * CIRCLE_MULTIPLIER;
		const float newStep = stepTheta / CIRCLE_MULTIPLIER;
		DrawRecursiveCircle(childPoint, newRadius, newStep, numChild, depth - 1);
	}
}

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ����
	static float moveSpeed = 100.f;

	// ���� ��⿡�� �Է� ������ ��������
	InputManager input = mGameEngine.GetInputManager();
	Vector2 deltaPosition = Vector2(input.GetXAxis(), input.GetYAxis()) * moveSpeed * InDeltaSeconds;
	mCurrentPosition += deltaPosition;

	if (input.IsSpacePressed())
	{
		mCurrentColor = LinearColor::Red;
		mCircleRadius += CIRCLE_EXPAND_SPEED * InDeltaSeconds;
		mCircleRotationRadian += CIRCLE_ROTATION_SPEED * InDeltaSeconds;
	}
	else
	{
		mCurrentColor = LinearColor::Blue;
		mCircleRadius = Math::Max(MIN_CIRCLE_RADIUS, mCircleRadius - CIRCLE_REDUCE_SPEED * InDeltaSeconds);
		mCircleRotationRadian -= CIRCLE_ROTATION_SPEED * InDeltaSeconds;
	}

	mCircleRotationRadian = Math::RadianClamp(mCircleRotationRadian);
}


// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	float oneDegree = Math::PI / (4 * mCircleRadius);
	const UINT32 currentCircleDepth = Math::CeilToInt(std::log(mCircleRadius) * 2.f);
	DrawRecursiveCircle(mCurrentPosition, mCircleRadius, oneDegree, 3, currentCircleDepth);

	// ���� ��ġ�� ȭ�鿡 ���
	mRSI->PushStatisticText(CIRCLE_ORIGIN_TEXT + mCurrentPosition.ToString());
	mRSI->PushStatisticText(CIRCLE_RADIUS_TEXT + std::to_string(mCircleRadius));
	mRSI->PushStatisticText(CIRCLE_DEPTH_TEXT + std::to_string(currentCircleDepth));
}

