#pragma once

// �� ���� ��
constexpr float MIN_CIRCLE_RADIUS = 1.f;
constexpr float CIRCLE_EXPAND_SPEED = 20.f;
constexpr float CIRCLE_REDUCE_SPEED = 30.f;
constexpr float CIRCLE_MULTIPLIER = 0.5f;
constexpr float CIRCLE_ROTATION_SPEED = 0.5f;

// ��� �ؽ�Ʈ
constexpr TCHAR* const CIRCLE_ORIGIN_TEXT = "CircleOrigin : ";
constexpr TCHAR* const CIRCLE_RADIUS_TEXT = "CircleRadius : ";
constexpr TCHAR* const CIRCLE_DEPTH_TEXT = "CircleDepth : ";


class SoftRenderer
{
public:
	// ������
	SoftRenderer(RenderingSoftwareInterface* InRSI);

public:
	// ������ �̺�Ʈ ó�� �Լ�
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

public:
	// ���α׷� �⺻ ����
	const ScreenPoint& GetScreenSize() { return mScreenSize; }
	float GetFrameFPS() const { return mFrameFPS; }
	float GetElapsedTime() const { return mElapsedTime; }

public:
	// ���� ������ ���� �Լ�
	std::function<float()> PerformanceInitFunc;
	std::function<INT64()> PerformanceMeasureFunc;

	// ƽ ó���� ���� �Լ�
	std::function<void()> RenderFrameFunc;
	std::function<void(float DeltaSeconds)> UpdateFunc;

	// ���� ���� ���۷���
	DD::GameEngine& GetGameEngine() { return mGameEngine; }

private:
	// �⺻ ���� �Լ�
	void PreUpdate();
	void PostUpdate();
	void Update();
	void RenderFrame();

private:
	// �ǽ��� ���� �ֿ� �Լ� ����
	void BindTickFunctions();

private:
	// 2D ���� �Լ�
	void Update2D(float InDeltaSeconds);
	void Render2D();
	void DrawGrid2D();

private:
	// ����� Ŀ���� ���� �Լ�
	void DrawBigPoint(const Vector2& targetPoint) const;
	void DrawCircle(float radius, float stepTheta) const;
	void DrawRecursiveCircle(const Vector2& origin, float radius, float stepTheta, UINT32 numChild, UINT32 depth) const;

private:
	// ����� Ŀ���� ���� ����
	float mCircleRadius = MIN_CIRCLE_RADIUS;
	float mCircleRotationRadian = 0.f;

private:
	int mGrid2DUnit = 10;
	
	// ���� ������ ���� ��� ����
	Vector2 mCurrentPosition = Vector2::Zero;
	LinearColor mCurrentColor = LinearColor::Blue;

private:
	// �ʱ�ȭ ���� ����
	bool mPerformanceCheckInitialized = false;
	bool mRendererInitialized = false;
	bool mGameEngineInitialized = false;
	bool mTickFunctionBound = false;
	bool mAllInitialized = false;

	// ȭ�� ũ��
	ScreenPoint mScreenSize;

	// ���� ���� ���� ������
	long long mStartTimeStamp = 0;
	long long mFrameTimeStamp = 0;
	long mFrameCount = 0;
	float mCyclesPerMilliSeconds = 0.f;
	float mFrameTime = 0.f;
	float mElapsedTime = 0.f;
	float mAverageFPS = 0.f;
	float mFrameFPS = 0.f;

	// ������ �������̽�
	std::unique_ptr<RenderingSoftwareInterface> mRSI;

	// ���� ����
	DD::GameEngine mGameEngine;
};
