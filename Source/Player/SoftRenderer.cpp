
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(RenderingSoftwareInterface* InRSI) : mRSI(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!mAllInitialized)
	{
		// �����ս� ī���� �ʱ�ȭ.
		if(PerformanceInitFunc && PerformanceMeasureFunc)
		{
			mCyclesPerMilliSeconds = PerformanceInitFunc();
			mPerformanceCheckInitialized = true;
		}
		else
		{
			return;
		}

		// ��ũ�� ũ�� Ȯ��
		if (mScreenSize.HasZero())
		{
			return;
		}

		// ����Ʈ ������ �ʱ�ȭ.
		if (!mRSI->Init(mScreenSize))
		{
			return;
		}

		mRendererInitialized = true;

		// ���� ���� �ʱ�ȭ
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

	// ũ�Ⱑ ����Ǹ� �������� ���� �ʱ�ȭ
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
	// ���� ���� ����.
	mFrameTimeStamp = PerformanceMeasureFunc();
	if (mFrameCount == 0)
	{
		mStartTimeStamp = mFrameTimeStamp;
	}

	// ��� �����.
	mRSI->Clear(LinearColor::Black);
}

void SoftRenderer::PostUpdate()
{
	// ������ ���� ����.
	RenderFrame();

	// ������ ������.
	mRSI->EndFrame();

	// ���� ���� ������.
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


