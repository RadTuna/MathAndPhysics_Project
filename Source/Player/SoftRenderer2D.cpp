
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// ������ ����
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

}

// ������ ����
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ���� �׸���
	DrawGrid2D();

	static const float outerRadius = 200.f;
	static const float innerRadius = 100.f;
	static const float step = Math::TwoPI / 5.f;
	static const float offset = Math::PI / 5.f;

	std::vector<Vector2> outerRing;
	std::vector<Vector2> innerRing;

	for (float angle = 0.f; angle < Math::TwoPI; angle += step)
	{
		const float outerX = std::cosf(angle) * outerRadius;
		const float outerY = std::sinf(angle) * outerRadius;
	    outerRing.emplace_back(outerY, outerX);

	    const float innerX = std::cosf(angle + offset) * innerRadius;
		const float innerY = std::sinf(angle + offset) * innerRadius;
		innerRing.emplace_back(innerY, innerX);
	}

	for (size_t index = 0; index < outerRing.size(); ++index)
	{
		size_t endIndex = (index + 1) % outerRing.size();

	    r.DrawLine(outerRing.at(index), innerRing.at(index), LinearColor::Magenta);
		r.DrawLine(innerRing.at(index), outerRing.at(endIndex), LinearColor::Magenta);
	}
}

