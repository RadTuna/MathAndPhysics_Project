
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : mRSI(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!mAllInitialized)
	{
		// 퍼포먼스 카운터 초기화.
		if(PerformanceInitFunc && PerformanceMeasureFunc)
		{
			mCyclesPerMilliSeconds = PerformanceInitFunc();
			mPerformanceCheckInitialized = true;
		}
		else
		{
			return;
		}

		// 스크린 크기 확인
		if (mScreenSize.HasZero())
		{
			return;
		}

		// 소프트 렌더러 초기화.
		if (!mRSI->Init(mScreenSize))
		{
			return;
		}

		mRendererInitialized = true;

		// 게임 엔진 초기화
		if (!mGameEngine.Init(mScreenSize))
		{
			return;
		}

		mGameEngineInitialized = true;

		mAllInitialized = mRendererInitialized && mPerformanceCheckInitialized && mGameEngineInitialized;
		if (mAllInitialized)
		{
			BindTickFunctions();
		}
	}
	else
	{
		assert(mRSI != nullptr && mRSI->IsInitialized() && !mScreenSize.HasZero());

		PreUpdate();
		Update();
		PostUpdate();
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	mScreenSize = InNewScreenSize;

	// 크기가 변경되면 렌더러와 엔진 초기화
	if (mRendererInitialized)
	{
		mRSI->Init(InNewScreenSize);
	}

	if (mGameEngineInitialized)
	{
		mGameEngine.Init(mScreenSize);
	}
	
}

void SoftRenderer::OnShutdown()
{
	mRSI->Shutdown();
}

void SoftRenderer::PreUpdate()
{
	// 성능 측정 시작.
	mFrameTimeStamp = PerformanceMeasureFunc();
	if (mFrameCount == 0)
	{
		mStartTimeStamp = mFrameTimeStamp;
	}

	// 배경 지우기.
	mRSI->Clear(LinearColor::Black);
}

void SoftRenderer::PostUpdate()
{
	// 렌더링 로직 수행.
	RenderFrame();

	// 렌더링 마무리.
	mRSI->EndFrame();

	// 성능 측정 마무리.
	mFrameCount++;
	const INT64 currentTimeStamp = PerformanceMeasureFunc();
	const INT64 frameCycles = currentTimeStamp - mFrameTimeStamp;
	const INT64 elapsedCycles = currentTimeStamp - mStartTimeStamp;
	mFrameTime = frameCycles / mCyclesPerMilliSeconds;
	mElapsedTime = elapsedCycles / mCyclesPerMilliSeconds;
	mFrameFPS = mFrameTime == 0.f ? 0.f : 1000.f / mFrameTime;
	mAverageFPS = mElapsedTime == 0.f ? 0.f : 1000.f / mElapsedTime * mFrameCount;
}

void SoftRenderer::RenderFrame()
{
	if (mTickFunctionBound)
	{
		RenderFrameFunc();
	}
}

void SoftRenderer::Update()
{
	if (mTickFunctionBound)
	{
		UpdateFunc(mFrameTime / 1000.f);
	}
}

void SoftRenderer::BindTickFunctions()
{
	using namespace std::placeholders;
	RenderFrameFunc = [this] { Render2D(); };
	UpdateFunc = [this](float deltaSeconds) { Update2D(deltaSeconds); };
	mTickFunctionBound = true;
}


