
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
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetWorldPosition();
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
	// �⺻ ���۷���
	GameEngine& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// �⺻ ���� ����
	static float orbitSpeedMercury = 300.f;
	static float rotateSpeedMercury = 200.f;
	static float orbitSpeedVenus = 80.f;
	static float rotateSpeedVenus = 50.f;
	static float orbitSpeedEarth = 100.f;
	static float rotateSpeedEarth = 30.f;
	static float orbitSpeedMoon = 500.f; // 30.f + 500.f = 530.f
	static float rotateSpeedMoon = 50.f;
	static float orbitSpeedMars = 100.f;
	static float rotateSpeedMars = 50.f;
	static float orbitSpeedDeimos = 300.f; // 50.f + 300.f = 350.f
	static float rotateSpeedDeimos = 20.f;
	static float orbitSpeedPhobos = 1000.f; // 50.f + 1000.f = 1050.f
	static float rotateSpeedPhobos = 10.f;

	// ���� ������Ʈ�� ī�޶� ������Ʈ
	GameObject& goMercury = g.GetGameObject(GameEngine::MercuryGo);
	GameObject& goMercuryPivot = g.GetGameObject(GameEngine::MercuryPivotGo);
	GameObject& goVenus = g.GetGameObject(GameEngine::VenusGo);
	GameObject& goVenusPivot = g.GetGameObject(GameEngine::VenusPivotGo);
	GameObject& goEarth = g.GetGameObject(GameEngine::EarthGo);
	GameObject& goEarthPivot = g.GetGameObject(GameEngine::EarthPivotGo);
	GameObject& goMoon = g.GetGameObject(GameEngine::MoonGo);
	GameObject& goMoonPivot = g.GetGameObject(GameEngine::MoonPivotGo);
	GameObject& goMars = g.GetGameObject(GameEngine::MarsGo);
	GameObject& goMarsPivot = g.GetGameObject(GameEngine::MarsPivotGo);
	GameObject& goDeimos = g.GetGameObject(GameEngine::DeimosGo);
	GameObject& goDeimosPivot = g.GetGameObject(GameEngine::DeimosPivotGo);
	GameObject& goPhobos = g.GetGameObject(GameEngine::PhobosGo);
	GameObject& goPhobosPivot = g.GetGameObject(GameEngine::PhobosPivotGo);

	// �� �༺�� ȸ�� �ο�
	goMercury.GetTransform().AddLocalRotation(rotateSpeedMercury * InDeltaSeconds);
	goMercuryPivot.GetTransform().AddLocalRotation(orbitSpeedMercury * InDeltaSeconds);
	goVenus.GetTransform().AddLocalRotation(rotateSpeedVenus * InDeltaSeconds);
	goVenusPivot.GetTransform().AddLocalRotation(orbitSpeedVenus * InDeltaSeconds);
	goEarth.GetTransform().AddLocalRotation(rotateSpeedEarth * InDeltaSeconds);
	goEarthPivot.GetTransform().AddLocalRotation(orbitSpeedEarth * InDeltaSeconds);
	goMoon.GetTransform().AddLocalRotation(rotateSpeedMoon * InDeltaSeconds);
	goMoonPivot.GetTransform().AddLocalRotation(orbitSpeedMoon * InDeltaSeconds);
	goMars.GetTransform().AddLocalRotation(rotateSpeedMars * InDeltaSeconds);
	goMarsPivot.GetTransform().AddLocalRotation(orbitSpeedMars * InDeltaSeconds);
	goDeimos.GetTransform().AddLocalRotation(rotateSpeedDeimos * InDeltaSeconds);
	goDeimosPivot.GetTransform().AddLocalRotation(orbitSpeedDeimos * InDeltaSeconds);
	goPhobos.GetTransform().AddLocalRotation(rotateSpeedPhobos * InDeltaSeconds);
	goPhobosPivot.GetTransform().AddLocalRotation(orbitSpeedPhobos * InDeltaSeconds);
}

// ������ ����
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ���� �׸���
	DrawGrid2D();

	// ī�޶��� �� ���
	Matrix3x3 viewMatrix = g.GetMainCamera().GetViewMatrix();

	// �����ϰ� ������ ��� ���� ������Ʈ��
	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		// ���� ������Ʈ�� �ʿ��� ���� ������ ��������
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const Texture& texture = g.GetTexture(gameObject.GetTextureKey());
		const TransformComponent& transform = gameObject.GetTransform();
		Matrix3x3 finalMatrix = viewMatrix * transform.GetWorldMatrix();

		DrawMesh2D(mesh, texture, finalMatrix, gameObject.GetColor());
	}
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Texture& InTexture, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
	std::vector<Vertex2D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = InMesh.GetVertices()[vi];
		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh.GetColors()[vi];
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh.GetUVs()[vi];
		}
	}

	// ���� ��ȯ ����
	VertexShader2D(vertices, InMatrix);

	// �׸����� ����
	FillMode fm = FillMode::None;
	if (InMesh.HasColor())
	{
		fm |= FillMode::Color;
	}
	if (InMesh.HasUV())
	{
		fm |= FillMode::Texture;
	}

	// �ﰢ�� ���� �׸���
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex2D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };
		DrawTriangle2D(tvs, InTexture, InColor, fm);
	}
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const Texture& InTexture, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get2DGameEngine();

	if (IsWireframeDrawing())
	{
		LinearColor finalColor = _WireframeColor;
		if (InColor != LinearColor::Error)
		{
			finalColor = InColor;
		}

		r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
		r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
		r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
	}
	else
	{
		// �ﰢ�� ĥ�ϱ�
		// �ﰢ���� ���� ����
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
		Vector2 u = InVertices[1].Position - InVertices[0].Position;
		Vector2 v = InVertices[2].Position - InVertices[0].Position;

		// ���� �и� �� ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// ��ȭ �ﰢ�� ����.
		if (Math::EqualsInTolerance(denominator, 0.f))
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// �� ���� ȭ�� ���� ����� ��� Ŭ���� ó��
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - InVertices[0].Position;
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					Vector2 targetUV = InVertices[0].UV * oneMinusST + InVertices[1].UV * s + InVertices[2].UV * t;
					r.DrawPoint(fragment, FragmentShader2D(InTexture.GetSample(targetUV), LinearColor::White));
				}
			}
		}
	}
}
