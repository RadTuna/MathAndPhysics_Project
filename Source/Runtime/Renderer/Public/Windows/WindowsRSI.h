
#pragma once

class WindowsGDI;
class WindowsRSI : public WindowsGDI, public RenderingSoftwareInterface
{
public:
	WindowsRSI() = default;
	~WindowsRSI();

public:
	bool Init(const ScreenPoint& InScreenSize) override;
	void Shutdown() override;

	[[nodiscard]]
	virtual bool IsInitialized() const { return mGDIInitialized; }

	void Clear(const LinearColor& InClearColor) override;
	void BeginFrame() override;
	void EndFrame() override;

	void DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor) override;

	void DrawFullVerticalLine(int InX, const LinearColor& InColor) override;
	void DrawFullHorizontalLine(int InY, const LinearColor& InColor) override;

	void PushStatisticText(std::string && InText) override;
	void PushStatisticTexts(std::vector<std::string> && InTexts) override;

private:
	FORCEINLINE void SetPixel(const ScreenPoint& InPos, const LinearColor& InColor);
	
};

FORCEINLINE void WindowsRSI::SetPixel(const ScreenPoint& InPos, const LinearColor& InColor)
{
	SetPixelOpaque(InPos, InColor);
}
