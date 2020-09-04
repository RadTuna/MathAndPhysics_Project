#pragma once

// 원 제어 값
constexpr float MIN_CIRCLE_RADIUS = 1.f;
constexpr float CIRCLE_EXPAND_SPEED = 20.f;
constexpr float CIRCLE_REDUCE_SPEED = 30.f;
constexpr float CIRCLE_MULTIPLIER = 0.5f;
constexpr float CIRCLE_ROTATION_SPEED = 0.5f;

// 출력 텍스트
constexpr TCHAR* const CIRCLE_ORIGIN_TEXT = "CircleOrigin : ";
constexpr TCHAR* const CIRCLE_RADIUS_TEXT = "CircleRadius : ";
constexpr TCHAR* const CIRCLE_DEPTH_TEXT = "CircleDepth : ";


class SoftRenderer
{
public:
	// 생성자
	SoftRenderer(RenderingSoftwareInterface* InRSI);

public:
	// 윈도우 이벤트 처리 함수
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

public:
	// 프로그램 기본 정보
	const ScreenPoint& GetScreenSize() { return mScreenSize; }
	float GetFrameFPS() const { return mFrameFPS; }
	float GetElapsedTime() const { return mElapsedTime; }

public:
	// 성능 측정을 위한 함수
	std::function<float()> PerformanceInitFunc;
	std::function<INT64()> PerformanceMeasureFunc;

	// 틱 처리를 위한 함수
	std::function<void()> RenderFrameFunc;
	std::function<void(float DeltaSeconds)> UpdateFunc;

	// 게임 엔진 레퍼런스
	DD::GameEngine& GetGameEngine() { return mGameEngine; }

private:
	// 기본 루프 함수
	void PreUpdate();
	void PostUpdate();
	void Update();
	void RenderFrame();

private:
	// 실습을 위한 주요 함수 연동
	void BindTickFunctions();

private:
	// 2D 구현 함수
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGrid2D();

private:
	// 사용자 커스텀 헬퍼 함수
	void DrawBigPoint(const Vector2& targetPoint) const;
	void DrawCircle(float radius, float stepTheta) const;
	void DrawRecursiveCircle(const Vector2& origin, float radius, float stepTheta, UINT32 numChild, UINT32 depth) const;

private:
	// 사용자 커스텀 헬퍼 변수
	float mCircleRadius = MIN_CIRCLE_RADIUS;
	float mCircleRotationRadian = 0.f;

private:
	int mGrid2DUnit = 10;
	
	// 로직 구현을 위한 멤버 변수
	Vector2 mCurrentPosition = Vector2::Zero;
	LinearColor mCurrentColor = LinearColor::Blue;

private:
	// 초기화 점검 변수
	bool mPerformanceCheckInitialized = false;
	bool mRendererInitialized = false;
	bool mGameEngineInitialized = false;
	bool mTickFunctionBound = false;
	bool mAllInitialized = false;

	// 화면 크기
	ScreenPoint mScreenSize;

	// 성능 측정 관련 변수들
	long long mStartTimeStamp = 0;
	long long mFrameTimeStamp = 0;
	long mFrameCount = 0;
	float mCyclesPerMilliSeconds = 0.f;
	float mFrameTime = 0.f;
	float mElapsedTime = 0.f;
	float mAverageFPS = 0.f;
	float mFrameFPS = 0.f;

	// 렌더러 인터페이스
	std::unique_ptr<RenderingSoftwareInterface> mRSI;

	// 게임 엔진
	DD::GameEngine mGameEngine;
};
