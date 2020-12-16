
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 기즈모 그리기
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetWorldPosition();
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

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 이름
static const std::string PlayerGo("Player");

// 씬 로딩
void SoftRenderer::LoadScene2D()
{
	GameEngine& g = Get2DGameEngine();

	// 플레이어
	constexpr float playerScale = 2.f;

	// 플레이어 설정
	GameObject& goPlayer = g.CreateNewGameObject(PlayerGo);
	goPlayer.SetMesh(GameEngine::CharacterMesh);
	goPlayer.GetTransform().SetWorldPosition(Vector2(0.f, -100.f * playerScale));
	goPlayer.GetTransform().SetWorldScale(Vector2::One * playerScale);
	goPlayer.SetColor(LinearColor::White);
}

// 실습을 위한 변수
Vector2 leftBonePosition;
Vector2 rightBonePosition;

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 플레이어의 이동
	static float moveSpeed = 100.f;
	GameObject& goPlayer = g.GetGameObject(PlayerGo);
	goPlayer.GetTransform().AddWorldPosition(Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds);
}

// 캐릭터 애니메이션 로직
void SoftRenderer::LateUpdate2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();

	// 기본 설정 변수
	static float duration = 3.f;
	static float elapsedTime = 0.f;

	// 애니메이션을 위한 커브 생성 ( 0~1 SineWave )
	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);
	if (elapsedTime == duration)
	{
		elapsedTime = 0.f;
	}
	const float waveParam = elapsedTime * Math::TwoPI / duration;
	const float sinWave = sinf(waveParam);
	const float cosWave = cosf(waveParam);

	// 플레이어의 이동
	GameObject& goPlayer = g.GetGameObject(PlayerGo);
	Mesh& m = g.GetMesh(goPlayer.GetMeshKey());
	if (m.IsSkinnedMesh())
	{
		if (m.HasBone("Shoulder_L"))
		{
			m.GetBone("Shoulder_L").GetTransform().AddWorldRotation(1.f * -sinWave);
		}
		if (m.HasBone("Elbow_L"))
		{
		    m.GetBone("Elbow_L").GetTransform().AddWorldRotation(1.5f * -sinWave);
		}

		if (m.HasBone("Shoulder_R"))
		{
			m.GetBone("Shoulder_R").GetTransform().AddWorldRotation(1.f * sinWave);
		}
		if (m.HasBone("Elbow_R"))
		{
		    m.GetBone("Elbow_R").GetTransform().AddWorldRotation(1.5f * sinWave);
		}

	    if (m.HasBone("Spine_0"))
		{
			m.GetBone("Spine_0").GetTransform().AddWorldRotation(1.5f * cosWave);
		}
		if (m.HasBone("Spine_1"))
		{
		    m.GetBone("Spine_1").GetTransform().AddWorldRotation(1.5f * cosWave);
		}
		if (m.HasBone("Head"))
		{
		    m.GetBone("Head").GetTransform().AddWorldRotation(1.5f * cosWave);
		}

	    if (m.HasBone("Femoral_L"))
		{
			m.GetBone("Femoral_L").GetTransform().AddWorldRotation(1.f * cosWave);
		}
		if (m.HasBone("Knee_L"))
		{
		    m.GetBone("Knee_L").GetTransform().AddWorldRotation(1.5f * cosWave);
		}
		if (m.HasBone("Ankle_L"))
		{
		    m.GetBone("Ankle_L").GetTransform().AddWorldRotation(1.f * cosWave);
		}

		if (m.HasBone("Femoral_R"))
		{
			m.GetBone("Femoral_R").GetTransform().AddWorldRotation(1.f * -cosWave);
		}
		if (m.HasBone("Knee_R"))
		{
		    m.GetBone("Knee_R").GetTransform().AddWorldRotation(1.5f * -cosWave);
		}
		if (m.HasBone("Ankle_R"))
		{
		    m.GetBone("Ankle_R").GetTransform().AddWorldRotation(1.f * -cosWave);
		}

		if (m.HasBone("Pelvis"))
		{
		    m.GetBone("Pelvis").GetTransform().AddWorldPosition(Vector2::UnitY * sinWave);
		}
	}
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 격자 그리기
	DrawGizmo2D();

	// 전체 그릴 물체의 수
	size_t totalObjectCount = g.GetScene().size();

	// 카메라의 뷰 행렬
	Matrix3x3 viewMatrix = g.GetMainCamera().GetViewMatrix();

	// 랜덤하게 생성된 모든 게임 오브젝트들
	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		// 게임 오브젝트에 필요한 내부 정보를 가져오기
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const TransformComponent& transform = gameObject.GetTransform();
		Matrix3x3 finalMatrix = viewMatrix * transform.GetWorldMatrix();
		DrawMesh2D(mesh, finalMatrix, gameObject.GetColor());
		DrawBone2D(mesh, finalMatrix, LinearColor::Red);
		
		if (gameObject == PlayerGo)
		{
			r.PushStatisticText("Player Scale : " + std::to_string(transform.GetWorldScale().X));
		}
	}
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
	std::vector<Vertex2D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = InMesh.GetVertices()[vi];

		// 위치에 대해 스키닝 연산 수행
		if (InMesh.IsSkinnedMesh())
		{
			Vector2 deltaPosition;
			Weight w = InMesh.GetWeights()[vi];
			for (size_t wi = 0; wi < InMesh.GetConnectedBones()[vi]; ++wi)
			{
				std::string boneName = w.Bones[wi];
				if (InMesh.HasBone(boneName))
				{
					const Bone& bone = InMesh.GetBone(boneName);
					const Transform2D& bindBoneTransform = bone.GetBindPose();
					const Transform2D& boneTransform = bone.GetTransform().GetWorldTransform();

					const float deltaRotation = boneTransform.GetRotation() - bindBoneTransform.GetRotation();
					const Vector2 RelativeVertexPosition = vertices[vi].Position - bindBoneTransform.GetPosition();
					Vector2 deltaVertexPosition = RelativeVertexPosition;
					float cos = 0.f;
					float sin = 0.f;
					Math::GetSinCos(sin, cos, deltaRotation);
					deltaVertexPosition = Vector2(
						deltaVertexPosition.Dot(Vector2(cos, -sin)), 
						deltaVertexPosition.Dot(Vector2(sin, cos)));
					deltaVertexPosition -= RelativeVertexPosition;

					deltaPosition += (boneTransform.GetPosition() - bindBoneTransform.GetPosition()) * w.Values[wi];
					deltaPosition += deltaVertexPosition * w.Values[wi];
				}
			}

			vertices[vi].Position += deltaPosition;
		}

		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh.GetColors()[vi];
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh.GetUVs()[vi];
		}
	}

	// 정점 변환 진행
	VertexShader2D(vertices, InMatrix);

	// 그리기모드 설정
	FillMode fm = FillMode::None;
	if (InMesh.HasColor())
	{
		fm |= FillMode::Color;
	}
	if (InMesh.HasUV())
	{
		fm |= FillMode::Texture;
	}

	// 삼각형 별로 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex2D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };
		DrawTriangle2D(tvs, InColor, fm);
	}
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get2DGameEngine();
	const Texture& texture = g.GetTexture(GameEngine::BaseTexture);

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
		// 삼각형 칠하기
		// 삼각형의 영역 설정
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// 무게중심좌표를 위해 점을 벡터로 변환
		Vector2 u = InVertices[1].Position - InVertices[0].Position;
		Vector2 v = InVertices[2].Position - InVertices[0].Position;

		// 공통 분모 값 ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// 퇴화 삼각형이면 그리기 생략
		if (denominator == 0.f)
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 삼각형 영역 내 모든 점을 점검하고 색칠
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
					r.DrawPoint(fragment, LinearColor::Black);
				}
			}
		}
	}
}

void SoftRenderer::DrawBone2D(const DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
	auto& r = GetRenderer();

	for (auto& bone : InMesh.GetBones())
	{
		if (bone.second.HasParent())
		{
	        Vector2 startPos = bone.second.GetTransform().GetWorldPosition();
		    const Bone& parentBone = InMesh.GetBones().at(bone.second.GetParentName());
			Vector2 endPos = parentBone.GetTransform().GetWorldPosition();

			startPos = InMatrix * startPos;
			endPos = InMatrix * endPos;

			r.DrawLine(startPos, endPos, InColor);

			// draw big point
			for (int x = -2; x <= 2; ++x)
			{
			    for (int y = -2; y <= 2; ++y)
			    {
			        const Vector2 drawPoint(startPos.X + x, startPos.Y + y);
					r.DrawPoint(drawPoint, LinearColor::Blue);
			    }
			}
		}
	}
}
