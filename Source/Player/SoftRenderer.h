#pragma once

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
