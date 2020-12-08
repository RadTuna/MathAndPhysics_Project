
#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");
const std::size_t GameEngine::CharacterMesh = std::hash<std::string>()("SM_Character");

// 게임 오브젝트
const std::string GameEngine::CharacterGo("CharacterMesh");

// 텍스쳐
const std::size_t GameEngine::CharacterTexture = std::hash<std::string>()("SunTex");

// 텍스쳐 경로
constexpr char* CharacterTexturePath = "Water.png";


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

	if (!LoadScene())
	{
		return false;
	}

	_IsInitialized = true;
	return true;
}

bool GameEngine::LoadResources()
{
	// 메시 데이터 로딩
	{
	    Mesh& quadMesh = CreateMesh(GameEngine::QuadMesh);

	    constexpr float squareHalfSize = 0.5f;
	    constexpr int vertexCount = 4;
	    constexpr int triangleCount = 2;
	    constexpr int indexCount = triangleCount * 3;

	    auto& v = quadMesh.GetVertices();
	    auto& i = quadMesh.GetIndices();
	    auto& uv = quadMesh.GetUVs();

	    v = {
		    Vector2(-squareHalfSize, -squareHalfSize),
		    Vector2(-squareHalfSize, squareHalfSize),
		    Vector2(squareHalfSize, squareHalfSize),
		    Vector2(squareHalfSize, -squareHalfSize)
	    };

	    uv = {
		    Vector2(0.f, 0.f),
		    Vector2(0.f, 1.f),
		    Vector2(1.f, 1.f),
		    Vector2(1.f, 0.f)
	    };

	    i = {
		    0, 2, 1, 0, 3, 2
	    };

	    quadMesh.CalculateBounds();
	}

	{
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

	    characterMesh.CalculateBounds();
	}

	// 텍스쳐 로딩
	if (!CreateTexture(GameEngine::CharacterTexture, CharacterTexturePath).IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static const float characterScale = 2.f;

	// CharacterMesh
	GameObject& goCharacter = CreateNewGameObject(GameEngine::CharacterGo);
	goCharacter.SetMesh(GameEngine::CharacterMesh);
	goCharacter.GetTransform().SetWorldScale(Vector2::One * characterScale);
	goCharacter.SetTexture(GameEngine::CharacterTexture);

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
