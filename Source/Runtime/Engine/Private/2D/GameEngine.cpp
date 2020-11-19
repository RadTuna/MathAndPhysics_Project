
#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");
const std::size_t GameEngine::StarMesh = std::hash<std::string>()("SM_Star");

// 게임 오브젝트
const std::string GameEngine::PlayerGo("Player");

// 텍스쳐
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::size_t GameEngine::WaterTexture = std::hash<std::string>()("Water");
const std::string GameEngine::SteveTexturePath("Steve.png");
const std::string GameEngine::WaterTexturePath("Water.png");

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
	{
	    // Quad 메시 데이터 로딩
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
		    Vector2(0.125f, 0.75f),
		    Vector2(0.125f, 0.875f),
		    Vector2(0.25f, 0.875f),
		    Vector2(0.25f, 0.75f)
	    };

	    i = {
		    0, 2, 1, 0, 3, 2
	    };

	    quadMesh.CalculateBounds();
	}

	{
	   	// Star 메시 데이터 로딩
	    Mesh& starMesh = CreateMesh(GameEngine::StarMesh);

		constexpr float innerRadius = 0.8f;
		constexpr float outerRadius = 2.f;

	    constexpr int vertexCount = 11;
	    constexpr int triangleCount = 10;
	    constexpr int indexCount = triangleCount * 3;

	    auto& v = starMesh.GetVertices();
	    auto& i = starMesh.GetIndices();
	    auto& uv = starMesh.GetUVs();

		const float step = Math::TwoPI / 5.f;
		const float innerOffset = Math::TwoPI / 10.f;

		v.emplace_back(Vector2(0.f, 0.f)); // Center Position

		for (size_t i = 0; i < 5; ++i)
		{
			const float innerAngle = step * i - innerOffset;
		    v.emplace_back(Vector2(std::cosf(innerAngle) * innerRadius, std::sinf(innerAngle) * innerRadius)); // Inner Position
		}

		for (size_t i = 0; i < 5; ++i)
		{
			const float outerAngle = step * i;
			v.emplace_back(Vector2(std::cosf(outerAngle) * outerRadius, std::sinf(outerAngle) * outerRadius)); // Outer Position
		}

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

	    i = {
		    0, 1, 2,  0, 2, 3,  0, 3, 4,  0, 4, 5,  0, 5, 1,  // Inner Triangle
			1, 6, 2,  2, 7, 3,  3, 8, 4,  4, 9, 5,  5, 10, 1  // Outer Triangle
	    };

	    starMesh.CalculateBounds(); 
	}

	// 텍스쳐 로딩
	Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
	if (!diffuseTexture.IsIntialized())
	{
		return false;
	}

	Texture& waterTexture = CreateTexture(GameEngine::WaterTexture, GameEngine::WaterTexturePath);
	if (!waterTexture.IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	constexpr float playerScale = 30.f;
	constexpr size_t starCount = 100;

	constexpr float minStarPos = -500.f;
	constexpr float maxStarPos = 500.f;
	constexpr float minStarScale = 10.f;
	constexpr float maxStarScale = 20.f;

	GameObject& playerGO = CreateNewGameObject(GameEngine::PlayerGo);
	playerGO.SetMesh(GameEngine::QuadMesh);
	playerGO.GetTransform().SetScale(Vector2::One * playerScale);
	playerGO.SetColor(LinearColor::Red);

	std::random_device randomDevice;
	std::mt19937 mersenneTwister(randomDevice());
	std::uniform_real_distribution<float> posDist(minStarPos, maxStarPos);
	std::uniform_real_distribution<float> scaleDist(minStarScale, maxStarScale);

	for (size_t i = 0; i < starCount; ++i)
	{
		char name[32];
		std::snprintf(name, sizeof(name), "Star%u", i);
		name[sizeof(name) - 1] = '\0';

		const float scale = scaleDist(mersenneTwister);
	    GameObject& star = CreateNewGameObject(name);
		star.SetMesh(GameEngine::StarMesh);
		star.GetTransform().SetPosition(Vector2(posDist(mersenneTwister), posDist(mersenneTwister)));
		star.GetTransform().SetScale(Vector2(scale, scale));
		star.SetColor(LinearColor::Blue);
	}

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
