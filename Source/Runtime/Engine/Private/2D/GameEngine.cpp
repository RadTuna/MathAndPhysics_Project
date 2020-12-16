
#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");
const std::size_t GameEngine::CharacterMesh = std::hash<std::string>()("SM_Character");

// 텍스처
const std::size_t GameEngine::BaseTexture = std::hash<std::string>()("Base");
const std::string GameEngine::SteveTexturePath("Steve.png");

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// 화면 크기의 설정
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// 이미 초기화되어 있으면 초기화 진행하지 않음.
	if (_IsInitialized)
	{
		return true;
	}

	// 화면 크기가 올바로 설정되어 있는지 확인
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputReady())
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	_IsInitialized = true;
	return true;
}

bool GameEngine::LoadResources()
{
	{
	    // 사각 메시
	    static float halfSize = 0.5f;
	    Mesh& quadMesh = CreateMesh(GameEngine::QuadMesh);
	    auto& v = quadMesh.GetVertices();
	    auto& i = quadMesh.GetIndices();
	    auto& uv = quadMesh.GetUVs();

	    v = {
		    Vector2(-1.f, -1.f) * halfSize, Vector2(-1.f, 1.f) * halfSize, Vector2(1.f, 1.f) * halfSize, Vector2(1.f, -1.f) * halfSize
	    };

	    i = {
		    0, 2, 1, 0, 3, 2
	    };

	    uv = {
		    Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f
	    };

	    // 스킨드 메시 설정
	    quadMesh.SetMeshType(MeshType::Skinned);

	    // 리깅 수행
	    auto& bones = quadMesh.GetBones();
	    auto& connectedBones = quadMesh.GetConnectedBones();
	    auto& weights = quadMesh.GetWeights();

	    bones = {
		    {"left", Bone("left", Transform2D(Vector2(-1.f, 0.f) * halfSize))},
		    {"right", Bone("right", Transform2D(Vector2(1.f, 0.f) * halfSize))}
	    };
	    connectedBones = { 1, 1, 1, 1 };
	    weights = {
		    { {"left"}, {1.f} },
		    { {"left"}, {1.f} },
		    { {"right"}, {1.f} },
		    { {"right"}, {1.f} }
	    };

	    // 메시의 바운딩 볼륨 생성
	    quadMesh.CalculateBounds();
	}

	{
		// 캐릭터 메쉬
	    Mesh& characterMesh = CreateMesh(GameEngine::CharacterMesh);

	    constexpr int vertexCount = 31;
	    constexpr int triangleCount = 19;
	    constexpr int indexCount = triangleCount * 3;

	    auto& v = characterMesh.GetVertices();
	    auto& i = characterMesh.GetIndices();
	    auto& uv = characterMesh.GetUVs();

		// make vertices
	    v = {
			Vector2(0.f, 140.f), // 1
			Vector2(-8.f, 130.f), // 2
			Vector2(8.f, 130.f), // 3
			Vector2(0.f, 120.f), // 4
			Vector2(-20.f, 115.f), // 5
			Vector2(-10.f, 100.f), // 6
			Vector2(10.f, 100.f), // 7
			Vector2(20.f, 115.f), // 8
			Vector2(0.f, 80.f), // 9
			Vector2(-15.f, 60.f), // 10
			Vector2(15.f, 60.f), // 11
			Vector2(10.f, 35.f), // 12
			Vector2(20.f, 35.f), // 13
			Vector2(15.f, 10.f), // 14
			Vector2(10.f, 0.f), // 15
			Vector2(20.f, 0.f), // 16
			Vector2(-20.f, 35.f), // 17
			Vector2(-10.f, 35.f), // 18
			Vector2(-15.f, 10.f), // 19
			Vector2(-20.f, 0.f), // 20
			Vector2(-10.f, 0.f), // 21
			Vector2(-45.f, 120.f), // 22
			Vector2(-45.f, 110.f), // 23
			Vector2(-70.f, 115.f), // 24
			Vector2(-80.f, 120.f), // 25
			Vector2(-80.f, 110.f), // 26
			Vector2(45.f, 110.f), // 27
			Vector2(45.f, 120.f), // 28
			Vector2(70.f, 115.f), // 29
			Vector2(80.f, 120.f), // 30
			Vector2(80.f, 110.f) // 31
	    };

		// make uvs
		auto GetUV = [](const Vector2& min, const Vector2& max, const Vector2& location) -> Vector2
		{
			const float u = (location.X - min.X) / max.X;
			const float v = (location.Y - min.Y) / max.Y;
			return Vector2(u, v);
		};

		Vector2 minBound(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Vector2 maxBound(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
		for (const Vector2& pos : v)
		{
		    if (minBound.X > pos.X)
		    {
		        minBound.X = pos.X;
		    }
			if (minBound.Y > pos.Y)
			{
			    minBound.Y = pos.Y;
			}

			if (maxBound.X < pos.X)
			{
			    maxBound.X = pos.X;
			}
			if (maxBound.Y < pos.Y)
			{
			    maxBound.Y = pos.Y;
			}
		}

		for (const Vector2& pos : v)
		{
		    uv.emplace_back(GetUV(minBound, maxBound, pos));
		}

		// make indices
	    i = {
		    0, 1, 2, // 1
			1, 3, 2, // 2
			3, 4, 5, // 3
			3, 5, 6, // 4
			3, 6, 7, // 5
			5, 6, 8, // 6
			8, 9, 10, // 7
			10, 11, 12, // 8
			11, 13, 12, // 9
			13, 14, 15, // 10
			9, 16, 17, // 11
			16, 18, 17, // 12
			18, 19, 20, // 13
			4, 21, 22, // 14
			21, 23, 22, // 15
			23, 24, 25, // 16
			7, 26, 27, // 17
			26, 27, 28, // 18
			28, 29, 30 // 19
	    };

	    // 스킨드 메시 설정
	    characterMesh.SetMeshType(MeshType::Skinned);

	    // 리깅 수행
	    auto& bones = characterMesh.GetBones();
	    auto& connectedBones = characterMesh.GetConnectedBones();
	    auto& weights = characterMesh.GetWeights();

		// 본 선언
		bones = {
		    { "Root", Bone("Root", Transform2D(Vector2::Zero, 0.f)) },
		    { "Pelvis", Bone("Pelvis", Transform2D(Vector2(0.f, 70.f), 0.f)) },
		    { "Spine_0", Bone("Spine_0", Transform2D(Vector2(0.f, 90.f), 0.f)) },
		    { "Spine_1", Bone("Spine_1", Transform2D(Vector2(0.f, 110.f), 0.f)) },
		    { "Head", Bone("Head", Transform2D(Vector2(0.f, 130.f), 0.f)) },
			{ "Shoulder_L", Bone("Shoulder_L", Transform2D(Vector2(-20.f, 115.f), 0.f)) },
		    { "Elbow_L", Bone("Elbow_L", Transform2D(Vector2(-45.f, 115.f), 0.f)) },
		    { "Wrist_L", Bone("Wrist_L", Transform2D(Vector2(-70.f, 115.f), 0.f)) },
		    { "Hand_L", Bone("Hand_L", Transform2D(Vector2(-80.f, 115.f), 0.f)) },
			{ "Shoulder_R", Bone("Shoulder_R", Transform2D(Vector2(20.f, 115.f), 0.f)) },
		    { "Elbow_R", Bone("Elbow_R", Transform2D(Vector2(45.f, 115.f), 0.f)) },
		    { "Wrist_R", Bone("Wrist_R", Transform2D(Vector2(70.f, 115.f), 0.f)) },
		    { "Hand_R", Bone("Hand_R", Transform2D(Vector2(80.f, 115.f), 0.f)) }, 
			{ "Femoral_L", Bone("Femoral_L", Transform2D(Vector2(-15.f, 60.f), 0.f)) },
		    { "Knee_L", Bone("Knee_L", Transform2D(Vector2(-15.f, 35.f), 0.f)) },
		    { "Ankle_L", Bone("Ankle_L", Transform2D(Vector2(-15.f, 10.f), 0.f)) },
		    { "Foot_L", Bone("Foot_L", Transform2D(Vector2(-15.f, 0.f), 0.f)) },
			{ "Femoral_R", Bone("Femoral_R", Transform2D(Vector2(15.f, 60.f), 0.f)) }, 
		    { "Knee_R", Bone("Knee_R", Transform2D(Vector2(15.f, 35.f), 0.f)) },
		    { "Ankle_R", Bone("Ankle_R", Transform2D(Vector2(15.f, 10.f), 0.f)) },
		    { "Foot_R",  Bone("Foot_R", Transform2D(Vector2(15.f, 0.f), 0.f)) } };
	    connectedBones = {
	        1, 1, 1, 2, // 4
			1, 2, 2, 1, // 8
			2, 2, 2, 1, // 12
			1, 2, 1, 1, // 16
			1, 1, 2, 1, // 20
			1, 1, 1, 2, // 24
			1, 1, 1, 1, // 28
			2, 1, 1 };
	    weights = {
		    { {"Head"}, {1.f} }, { {"Head"}, {1.f} },
	        { {"Head"}, {1.f} }, { {"Head", "Spine_1"}, {0.5f, 0.5f} },
		    { {"Shoulder_L"}, {1.f} }, { {"Spine_0", "Spine_1"}, {0.5f, 0.5f} },
	        { {"Spine_0", "Spine_1"}, {0.5f, 0.5f} }, { {"Shoulder_R"}, {1.f} },
		    { {"Pelvis", "Spine_0"}, {0.5f, 0.5f} }, { {"Pelvis", "Femoral_L"}, {0.5f, 0.5f} },
	        { {"Pelvis", "Femoral_R"}, {0.5f, 0.5f} }, { {"Knee_R"}, {1.f} },
			{ {"Knee_R"}, {1.f} }, { {"Knee_R", "Ankle_R"}, {0.5f, 0.5f} },
	        { {"Foot_R"}, {1.f} }, { {"Foot_R"}, {1.f} },
			{ {"Knee_L"}, {1.f} }, { {"Knee_L"}, {1.f} },
	        { {"Knee_L", "Ankle_L"}, {0.5f, 0.5f} }, { {"Foot_L"}, {1.f} },
			{ {"Foot_L"}, {1.f} }, { {"Elbow_L"}, {1.f} },
	        { {"Elbow_L"}, {1.f} }, { {"Elbow_L", "Wrist_L"}, {0.5f, 0.5f} },
			{ {"Hand_L"}, {1.f} }, { {"Hand_L"}, {1.f} },
	        { {"Elbow_R"}, {1.f} }, { {"Elbow_R"}, {1.f} },
			{ {"Elbow_R", "Wrist_R"}, {0.5f, 0.5f} }, { {"Hand_R"}, {1.f} },
	        { {"Hand_R"}, {1.f} } };

		// 본 계층 구조 설정
		Bone& Root = bones["Root"];
		Bone& Pelvis = bones["Pelvis"];
		Bone& Spine_0 = bones["Spine_0"];
		Bone& Spine_1 = bones["Spine_1"];
		Bone& Head = bones["Head"];
		Bone& Shoulder_L = bones["Shoulder_L"];
		Bone& Elbow_L = bones["Elbow_L"];
		Bone& Wrist_L = bones["Wrist_L"];
		Bone& Hand_L = bones["Hand_L"];
		Bone& Shoulder_R = bones["Shoulder_R"];
		Bone& Elbow_R = bones["Elbow_R"];
		Bone& Wrist_R = bones["Wrist_R"];
		Bone& Hand_R = bones["Hand_R"];
		Bone& Femoral_L = bones["Femoral_L"];
		Bone& Knee_L = bones["Knee_L"];
		Bone& Ankle_L = bones["Ankle_L"];
		Bone& Foot_L = bones["Foot_L"];
		Bone& Femoral_R = bones["Femoral_R"];
		Bone& Knee_R = bones["Knee_R"];
		Bone& Ankle_R = bones["Ankle_R"];
		Bone& Foot_R = bones["Foot_R"];

		Foot_R.SetParent(Ankle_R);
		Foot_L.SetParent(Ankle_L);
		Ankle_R.SetParent(Knee_R);
		Ankle_L.SetParent(Knee_L);
		Knee_R.SetParent(Femoral_R);
		Knee_L.SetParent(Femoral_L);
		Femoral_R.SetParent(Pelvis);
		Femoral_L.SetParent(Pelvis);

		Hand_R.SetParent(Wrist_R);
		Hand_L.SetParent(Wrist_L);
		Wrist_R.SetParent(Elbow_R);
		Wrist_L.SetParent(Elbow_L);
		Elbow_R.SetParent(Shoulder_R);
		Elbow_L.SetParent(Shoulder_L);
		Shoulder_R.SetParent(Spine_1);
		Shoulder_L.SetParent(Spine_1);

		Head.SetParent(Spine_1);
		Spine_1.SetParent(Spine_0);
		Spine_0.SetParent(Pelvis);
		Pelvis.SetParent(Root);

	    characterMesh.CalculateBounds();
	}

	// 텍스쳐 로딩
	Texture& diffuseTexture = CreateTexture(GameEngine::BaseTexture, GameEngine::SteveTexturePath);
	assert(diffuseTexture.IsIntialized());

	return true;
}

Mesh& GameEngine::CreateMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<Mesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return *_Meshes.at(InKey).get();
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey, const std::string& InTexturePath)
{
	auto texturePtr = std::make_unique<Texture>(InTexturePath);
	_Textures.insert({ InKey, std::move(texturePtr) });
	return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			assert(false);
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	return (it != _Scene.end()) ? *(*it).get() : GameObject::Invalid;
}
