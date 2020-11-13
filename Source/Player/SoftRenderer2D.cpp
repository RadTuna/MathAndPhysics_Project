
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
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

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 실습을 위한 변수
Vector2 deltaPosition;
float deltaDegree = 0.f;
float currentScale = 10.f;

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직에만 사용하는 변수
	static float moveSpeed = 100.f;
	static float scaleMin = 100.f;
	static float scaleMax = 200.f;
	static float scaleSpeed = 20.f;
	static float rotateSpeed = 180.f;

	// 입력 값으로 데이터 변경
	deltaPosition = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;
	currentScale = Math::Clamp(currentScale + deltaScale, scaleMin, scaleMax);
	deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	RendererInterface& r = GetRenderer();
	const GameEngine& g = Get2DGameEngine();
	const Texture& texture = g.GetTexture(GameEngine::WaterTexture);
	const Mesh& mesh = g.GetMesh(GameEngine::QuadMesh);

	// 격자 그리기
	DrawGrid2D();

	// 렌더링 관련 변수
	static Vector2 currentPosition;
	static float currentDegree = 0.f;
	currentPosition += deltaPosition;
	currentDegree += deltaDegree;

	static float uvAnimationSpeed = 0.01f;

	// 정점 배열과 인덱스 배열 생성
	static std::vector<Vector2> positions = mesh.GetVertices();
	static std::vector<Vector2> uvs = mesh.GetUVs();
	static std::vector<LinearColor> colors = mesh.GetColors();
	static std::vector<size_t> indices = mesh.GetIndices();

	// 메시 데이터
	static size_t vertexCount = positions.size();
	static size_t triangleCount = indices.size() / 3;

	// 아핀 변환 행렬 ( 크기 ) 
	Vector3 sBasis1(currentScale, 0.f, 0.f);
	Vector3 sBasis2(0.f, currentScale, 0.f);
	Vector3 sBasis3 = Vector3::UnitZ;
	Matrix3x3 sMatrix(sBasis1, sBasis2, sBasis3);

	// 아핀 변환 행렬 ( 회전 ) 
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, currentDegree);
	Vector3 rBasis1(cos, sin, 0.f);
	Vector3 rBasis2(-sin, cos, 0.f);
	Vector3 rBasis3 = Vector3::UnitZ;
	Matrix3x3 rMatrix(rBasis1, rBasis2, rBasis3);

	// 아핀 변환 행렬 ( 이동 ) 
	Vector3 tBasis1 = Vector3::UnitX;
	Vector3 tBasis2 = Vector3::UnitY;
	//Vector3 tBasis3(currentPosition.X, currentPosition.Y, 1.f);
	Vector3 tBasis3 = Vector3::UnitZ;
	Matrix3x3 tMatrix(tBasis1, tBasis2, tBasis3);

	// 모든 아핀 변환의 조합 행렬. 크기-회전-이동 순으로 조합
	Matrix3x3 finalMatrix = tMatrix * rMatrix * sMatrix;

	// 정점에 행렬을 적용
	static std::vector<Vertex2D> vertices(vertexCount);
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = finalMatrix * positions[vi];
	}

	if (mesh.HasColor())
	{
	    for (size_t vi = 0; vi < vertexCount; ++vi)
	    {
		    vertices[vi].Color = colors[vi];
	    }
	}

	if (mesh.HasUV())
	{
	    for (size_t vi = 0; vi < vertexCount; ++vi)
	    {
		    vertices[vi].UV = uvs[vi];
	    }
	}

	vertices[0].Color = LinearColor::Red;
	vertices[1].Color = LinearColor::Blue;
	vertices[2].Color = LinearColor::Green;
	vertices[3].Color = LinearColor::Blue;


	// 변환된 정점을 잇는 선 그리기
	for (size_t ti = 0; ti < triangleCount; ++ti)
	{
		size_t bi = ti * 3;
		std::array<Vertex2D, 3> tv = { vertices[indices[bi]] , vertices[indices[bi + 1]], vertices[indices[bi + 2]] };

		const Vector2 vectorU = tv[0].Position - tv[2].Position;
		const Vector2 vectorV = tv[1].Position - tv[2].Position;
		const float dotUV = vectorU.Dot(vectorV);
		const float dotUU = vectorU.Dot(vectorU);
		const float dotVV = vectorV.Dot(vectorV);
		const float denominator = 1.f / (dotUV * dotUV - dotUU * dotVV);

		Vector2 minPos(Math::Min3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Min3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));
		Vector2 maxPos(Math::Max3(tv[0].Position.X, tv[1].Position.X, tv[2].Position.X), Math::Max3(tv[0].Position.Y, tv[1].Position.Y, tv[2].Position.Y));

		// 화면상의 점 구하기
		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 삼각형을 둘러싼 사각형 영역의 점을 모두 Loop
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);

				// 해당 점이 컨벡스 조건을 만족할 때만 점 찍기
				const Vector2 vectorW = pointToTest - tv[2].Position;

				const float dotWU = vectorW.Dot(vectorU);
				const float dotWV = vectorW.Dot(vectorV);

				const float weightS = (dotWV * dotUV - dotWU * dotVV) * denominator;
				const float weightT = (dotWU * dotUV - dotWV * dotUU) * denominator;
				const float weightU = 1.f - weightS - weightT;

				const bool bIsBoundS = weightS >= 0.f && weightS <= 1.f;
				const bool bIsBoundT = weightT >= 0.f && weightT <= 1.f;
				const bool bIsBoundU = weightU >= 0.f && weightU <= 1.f;
				if (bIsBoundS && bIsBoundT && bIsBoundU)
				{
					Vector2 uv = tv[0].UV * weightS + tv[1].UV * weightT + tv[2].UV * weightU;

					uv.X = uv.X + currentPosition.X * uvAnimationSpeed;
					uv.Y = uv.Y + currentPosition.Y * uvAnimationSpeed;

				    r.DrawPoint(pointToTest, texture.GetSample(uv));
				}
			}
		}
	}

	// 현재 위치, 스케일, 회전각을 화면에 출력
	r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
