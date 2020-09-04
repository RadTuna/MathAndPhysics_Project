
#pragma once

class WindowsGDI
{
public:
	WindowsGDI() = default;
	~WindowsGDI();

public:
	bool InitializeGDI(const ScreenPoint& InScreenSize);
	void ReleaseGDI();

	void FillBuffer(Color32 InColor);

	FORCEINLINE LinearColor GetPixel(const ScreenPoint& InPos);
	FORCEINLINE void SetPixelOpaque(const ScreenPoint& InPos, const LinearColor& InColor);
	FORCEINLINE void SetPixelAlphaBlending(const ScreenPoint& InPos, const LinearColor& InColor);

	void CreateDepthBuffer();
	void ClearDepthBuffer();

	[[nodiscard]]
	float GetDepthBufferValue(const ScreenPoint& InPos) const;
	void SetDepthBufferValue(const ScreenPoint& InPos, float InDepthValue);

	[[nodiscard]]
	Color32* GetScreenBuffer() const;

	void DrawStatisticTexts();

	void SwapBuffer();

protected:
	FORCEINLINE bool IsInScreen(const ScreenPoint& InPos) const;

	[[nodiscard]]
	int GetScreenBufferIndex(const ScreenPoint& InPos) const;

	template <class T>
	T* CopyBuffer(T* InDst, T* InSrc, int InCount);

protected:
	bool mGDIInitialized = false;

	HWND mHandle = nullptr;
	HDC	mScreenDC = nullptr;
	HDC mMemoryDC = nullptr;
	HBITMAP mDefaultBitmap = nullptr;
	HBITMAP mDIBitmap = nullptr;

	Color32* mScreenBuffer = nullptr;
	float* mDepthBuffer = nullptr;

	ScreenPoint mScreenSize;
	std::vector<std::string> mStatisticTexts;
};

FORCEINLINE void WindowsGDI::SetPixelOpaque(const ScreenPoint& InPos, const LinearColor& InColor)
{
	if (!IsInScreen(InPos))
	{
		return;
	}

	Color32* dest = mScreenBuffer;
	*(dest + GetScreenBufferIndex(InPos)) = InColor.ToColor32();
	return;
}

FORCEINLINE void WindowsGDI::SetPixelAlphaBlending(const ScreenPoint & InPos, const LinearColor & InColor)
{
	const LinearColor bufferColor = GetPixel(InPos);
	if (!IsInScreen(InPos))
	{
		return;
	}

	Color32* dest = mScreenBuffer;
	*(dest + GetScreenBufferIndex(InPos)) = (InColor * InColor.A + bufferColor * (1.f - InColor.A)).ToColor32();
}

FORCEINLINE bool WindowsGDI::IsInScreen(const ScreenPoint& InPos) const
{
	if ((InPos.X < 0 || InPos.X >= mScreenSize.X) || (InPos.Y < 0 || InPos.Y >= mScreenSize.Y))
	{
		return false;
	}

	return true;
}

FORCEINLINE int WindowsGDI::GetScreenBufferIndex(const ScreenPoint& InPos) const
{
	return InPos.Y * mScreenSize.X + InPos.X;
}

FORCEINLINE LinearColor WindowsGDI::GetPixel(const ScreenPoint& InPos)
{
	if (!IsInScreen(InPos))
	{
		return LinearColor::Error;
	}

	Color32* dest = mScreenBuffer;
	const Color32 bufferColor = *(dest + GetScreenBufferIndex(InPos));
	return LinearColor(bufferColor);
}