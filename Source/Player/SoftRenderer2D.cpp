
#include "Precompiled.h"
#include "SoftRenderer.h"

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 가로 세로 라인 그리기
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

	// 월드 축 그리기
	mRSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	mRSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}

void SoftRenderer::DrawBigPoint(const Vector2& targetPoint) const
{
	// 지정한 점을 기준으로 상하좌우로 점 찍기
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

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에만 사용하는 변수
	static float moveSpeed = 100.f;

	// 엔진 모듈에서 입력 관리자 가져오기
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


// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	float oneDegree = Math::PI / (4 * mCircleRadius);
	const UINT32 currentCircleDepth = Math::CeilToInt(std::log(mCircleRadius) * 2.f);
	DrawRecursiveCircle(mCurrentPosition, mCircleRadius, oneDegree, 3, currentCircleDepth);

	// 현재 위치를 화면에 출력
	mRSI->PushStatisticText(CIRCLE_ORIGIN_TEXT + mCurrentPosition.ToString());
	mRSI->PushStatisticText(CIRCLE_RADIUS_TEXT + std::to_string(mCircleRadius));
	mRSI->PushStatisticText(CIRCLE_DEPTH_TEXT + std::to_string(currentCircleDepth));
}

