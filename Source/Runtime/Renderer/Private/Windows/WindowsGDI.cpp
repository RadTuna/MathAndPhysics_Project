
#include "Precompiled.h"

WindowsGDI::~WindowsGDI()
{
}

bool WindowsGDI::InitializeGDI(const ScreenPoint& InScreenSize)
{
	ReleaseGDI();

	mHandle = ::GetActiveWindow();
	if (mHandle == nullptr)
	{
		return false;
	}

	if (mGDIInitialized)
	{
		DeleteObject(mDefaultBitmap);
		DeleteObject(mDIBitmap);
		ReleaseDC(mHandle, mScreenDC);
		ReleaseDC(mHandle, mMemoryDC);
	}

	mScreenDC = GetDC(mHandle);
	if (mScreenDC == nullptr)
	{
		return false;
	}

	mMemoryDC = CreateCompatibleDC(mScreenDC);
	if (mMemoryDC == nullptr)
	{
		return false;
	}

	mScreenSize = InScreenSize;

	// Color & Bitmap Setting
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = mScreenSize.X;
	bmi.bmiHeader.biHeight = -mScreenSize.Y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	mDIBitmap = CreateDIBSection(mMemoryDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&mScreenBuffer), nullptr, 0);
	if (mDIBitmap == nullptr)
	{
		return false;
	}

	mDefaultBitmap = static_cast<HBITMAP>(SelectObject(mMemoryDC, mDIBitmap));
	if (mDefaultBitmap == nullptr)
	{
		return false;
	}

	// Create Depth Buffer
	CreateDepthBuffer();

	mGDIInitialized = true;
	return true;
}

void WindowsGDI::ReleaseGDI()
{
	if (mGDIInitialized)
	{
		DeleteObject(mDefaultBitmap);
		DeleteObject(mDIBitmap);
		ReleaseDC(mHandle, mScreenDC);
		ReleaseDC(mHandle, mMemoryDC);
	}

	if (mDepthBuffer != nullptr)
	{
		delete[] mDepthBuffer;
		mDepthBuffer = nullptr;
	}

	mGDIInitialized = false;
}


void WindowsGDI::FillBuffer(Color32 InColor)
{
	if (!mGDIInitialized || (mScreenBuffer == nullptr))
	{
		return;
	}

	Color32* dest = mScreenBuffer;
	const UINT32 totalCount = mScreenSize.X * mScreenSize.Y;
	CopyBuffer<Color32>(mScreenBuffer, &InColor, totalCount);
	return;
}

template <class T>
T* WindowsGDI::CopyBuffer(T* InDst, T* InSrc, int InCount)
{
	if (InCount == 0)
	{
		return nullptr;
	}

	if (InCount == 1)
	{
		memcpy(InDst, InSrc, sizeof(T));
	}
	else
	{
		int half = Math::FloorToInt(static_cast<float>(InCount) * 0.5f);
		CopyBuffer<T>(InDst, InSrc, half);
		memcpy(InDst + half, InDst, half * sizeof(T));

		if (InCount % 2)
		{
			memcpy(InDst + (InCount - 1), InSrc, sizeof(T));
		}
	}

	return InDst;
}

Color32* WindowsGDI::GetScreenBuffer() const
{
	return mScreenBuffer;
}

void WindowsGDI::DrawStatisticTexts()
{
	if (mStatisticTexts.empty())
	{
		return;
	}

	HFONT hOldFont;
	const HFONT hFont = static_cast<HFONT>(GetStockObject(ANSI_VAR_FONT));
	if (hOldFont = static_cast<HFONT>(SelectObject(mMemoryDC, hFont)))
	{
		static const int leftPosition = 600;
		static const int topPosition = 10;
		static const int rowHeight = 20;
		int currentPosition = topPosition;
		for (std::vector<std::string>::const_iterator it = mStatisticTexts.begin(); it != mStatisticTexts.end(); ++it)
		{
			TextOut(mMemoryDC, leftPosition, currentPosition, (*it).c_str(), static_cast<int>((*it).length()));
			currentPosition += rowHeight;
		}

		SelectObject(mMemoryDC, hOldFont);
	}
}

void WindowsGDI::SwapBuffer()
{
	if (!mGDIInitialized)
	{
		return;
	}

	DrawStatisticTexts();
	BitBlt(mScreenDC, 0, 0, mScreenSize.X, mScreenSize.Y, mMemoryDC, 0, 0, SRCCOPY);

	mStatisticTexts.clear();
}

void WindowsGDI::CreateDepthBuffer()
{
	mDepthBuffer = new float[mScreenSize.X * mScreenSize.Y];
}

void WindowsGDI::ClearDepthBuffer()
{
	if (mDepthBuffer != nullptr)
	{
		float* dest = mDepthBuffer;
		float defValue = INFINITY;
		const UINT32 totalCount = mScreenSize.X * mScreenSize.Y;
		CopyBuffer<float>(mDepthBuffer, &defValue, totalCount);
	}
}

float WindowsGDI::GetDepthBufferValue(const ScreenPoint& InPos) const
{
	if (mDepthBuffer == nullptr)
	{
		return INFINITY;
	}

	if (!IsInScreen(InPos))
	{
		return INFINITY;
	}

	return *(mDepthBuffer + GetScreenBufferIndex(InPos));
}

void WindowsGDI::SetDepthBufferValue(const ScreenPoint& InPos, float InDepthValue)
{
	if (mDepthBuffer == nullptr)
	{
		return;
	}

	if (!IsInScreen(InPos))
	{
		return;
	}

	*(mDepthBuffer + GetScreenBufferIndex(InPos)) = InDepthValue;
}