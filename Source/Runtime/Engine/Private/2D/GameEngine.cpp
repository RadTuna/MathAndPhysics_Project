
#include "Precompiled.h"
#include <random>
using namespace CK::DD;

// 메시
const std::size_t GameEngine::QuadMesh = std::hash<std::string>()("SM_Quad");

// 게임 오브젝트
const std::string GameEngine::SunGo("Sun");
const std::string GameEngine::MercuryPivotGo("MercuryPivot");
const std::string GameEngine::MercuryGo("Mercury");
const std::string GameEngine::VenusPivotGo("VenusPivot");
const std::string GameEngine::VenusGo("Venus");
const std::string GameEngine::EarthPivotGo("EarthPivot");
const std::string GameEngine::EarthGo("Earth");
const std::string GameEngine::MoonPivotGo("MoonPivot");
const std::string GameEngine::MoonGo("Moon");
const std::string GameEngine::MarsPivotGo("MarsPivot");
const std::string GameEngine::MarsGo("Mars");
const std::string GameEngine::DeimosPivotGo("DeimosPivot");
const std::string GameEngine::DeimosGo("Deimos");
const std::string GameEngine::PhobosPivotGo("PhobosPivot");
const std::string GameEngine::PhobosGo("Phobos");

// 텍스쳐
const std::size_t GameEngine::SunTexture = std::hash<std::string>()("SunTex");
const std::size_t GameEngine::MercuryTexture = std::hash<std::string>()("MercuryTex");
const std::size_t GameEngine::VenusTexture = std::hash<std::string>()("VenusTex");
const std::size_t GameEngine::EarthTexture = std::hash<std::string>()("EarthTex");
const std::size_t GameEngine::MoonTexture = std::hash<std::string>()("MoonTex");
const std::size_t GameEngine::MarsTexture = std::hash<std::string>()("MarsTex");
const std::size_t GameEngine::DeimosTexture = std::hash<std::string>()("DeimosTex");
const std::size_t GameEngine::PhobosTexture = std::hash<std::string>()("PhobosTex");

// 텍스쳐 경로
constexpr char* SunTexturePath = "Sun.png";
constexpr char* MercuryTexturePath = "Mercury.png";
constexpr char* VenusTexturePath = "Venus.png";
constexpr char* EarthTexturePath = "Earth.png";
constexpr char* MoonTexturePath = "Moon.png";
constexpr char* MarsTexturePath = "Mars.png";
constexpr char* DeimosTexturePath = "Deimos.png";
constexpr char* PhobosTexturePath = "Phobos.png";


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

	// 텍스쳐 로딩
	if (!CreateTexture(GameEngine::SunTexture, SunTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::MercuryTexture, MercuryTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::VenusTexture, VenusTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::EarthTexture, EarthTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::MoonTexture, MoonTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::MarsTexture, MarsTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::DeimosTexture, DeimosTexturePath).IsIntialized())
	{
		return false;
	}

	if (!CreateTexture(GameEngine::PhobosTexture, PhobosTexturePath).IsIntialized())
	{
		return false;
	}

	return true;
}

bool GameEngine::LoadScene()
{
	static const float sunScale = 70.f;
	static const float mercuryScale = 15.f;
	static const float venusScale = 20.f;
	static const float earthScale = 25.f;
	static const float moonScale = 10.f;
	static const float marsScale = 18.f;
	static const float deimosScale = 10.f;
	static const float phobosScale = 10.f;

	static const Vector2 mercuryOffset(70.f, 0.f);
	static const Vector2 venusOffset(130.f, 0.f);
	static const Vector2 earthOffset(200.f, 0.0f);
	static const Vector2 moonOffset(230.f, 0.0f);
	static const Vector2 marsOffset(300.f, 0.f);
	static const Vector2 deimosOffset(320.f, 0.f);
	static const Vector2 phobosOffset(340.f, 0.f);

	// Sun
	GameObject& goSun = CreateNewGameObject(GameEngine::SunGo);
	goSun.SetMesh(GameEngine::QuadMesh);
	goSun.GetTransform().SetWorldScale(Vector2::One * sunScale);
	goSun.SetTexture(GameEngine::SunTexture);

	// Mercury
	GameObject& goMercuryPivot = CreateNewGameObject(GameEngine::MercuryPivotGo);
	goMercuryPivot.GetTransform().SetWorldPosition(goSun.GetTransform().GetWorldPosition());
	goMercuryPivot.SetParent(goSun);

	GameObject& goMercury = CreateNewGameObject(GameEngine::MercuryGo);
	goMercury.SetMesh(GameEngine::QuadMesh);
	goMercury.GetTransform().SetWorldPosition(mercuryOffset);
	goMercury.GetTransform().SetWorldScale(Vector2::One * mercuryScale);
	goMercury.SetParent(goMercuryPivot);
	goMercury.SetTexture(GameEngine::MercuryTexture);

	// Venus
	GameObject& goVenusPivot = CreateNewGameObject(GameEngine::VenusPivotGo);
	goVenusPivot.GetTransform().SetWorldPosition(goSun.GetTransform().GetWorldPosition());
	goVenusPivot.SetParent(goSun);

	GameObject& goVenus = CreateNewGameObject(GameEngine::VenusGo);
	goVenus.SetMesh(GameEngine::QuadMesh);
	goVenus.GetTransform().SetWorldPosition(venusOffset);
	goVenus.GetTransform().SetWorldScale(Vector2::One * venusScale);
	goVenus.SetParent(goVenusPivot);
	goVenus.SetTexture(GameEngine::VenusTexture);

	// Earth
	GameObject& goEarthPivot = CreateNewGameObject(GameEngine::EarthPivotGo);
	goEarthPivot.GetTransform().SetWorldPosition(goSun.GetTransform().GetWorldPosition());
	goEarthPivot.SetParent(goSun);

	GameObject& goEarth = CreateNewGameObject(GameEngine::EarthGo);
	goEarth.SetMesh(GameEngine::QuadMesh);
	goEarth.GetTransform().SetWorldPosition(earthOffset);
	goEarth.GetTransform().SetWorldScale(Vector2::One * earthScale);
	goEarth.SetParent(goEarthPivot);
	goEarth.SetTexture(GameEngine::EarthTexture);

	// Moon
	GameObject& goMoonPivot = CreateNewGameObject(GameEngine::MoonPivotGo);
	goMoonPivot.GetTransform().SetWorldPosition(goEarth.GetTransform().GetWorldPosition());
	goMoonPivot.SetParent(goEarth);

	GameObject& goMoon = CreateNewGameObject(GameEngine::MoonGo);
	goMoon.SetMesh(GameEngine::QuadMesh);
	goMoon.GetTransform().SetWorldPosition(moonOffset);
	goMoon.GetTransform().SetWorldScale(Vector2::One * moonScale);
	goMoon.SetParent(goMoonPivot);
	goMoon.SetTexture(GameEngine::MoonTexture);

	// Mars
	GameObject& goMarsPivot = CreateNewGameObject(GameEngine::MarsPivotGo);
	goMarsPivot.GetTransform().SetWorldPosition(goSun.GetTransform().GetWorldPosition());
	goMarsPivot.SetParent(goSun);

	GameObject& goMars = CreateNewGameObject(GameEngine::MarsGo);
	goMars.SetMesh(GameEngine::QuadMesh);
	goMars.GetTransform().SetWorldPosition(marsOffset);
	goMars.GetTransform().SetWorldScale(Vector2::One * marsScale);
	goMars.SetParent(goMarsPivot);
	goMars.SetTexture(GameEngine::MarsTexture);

	// Deimos
	GameObject& goDeimosPivot = CreateNewGameObject(GameEngine::DeimosPivotGo);
	goDeimosPivot.GetTransform().SetWorldPosition(goMars.GetTransform().GetWorldPosition());
	goDeimosPivot.SetParent(goMars);

	GameObject& goDeimos = CreateNewGameObject(GameEngine::DeimosGo);
	goDeimos.SetMesh(GameEngine::QuadMesh);
	goDeimos.GetTransform().SetWorldPosition(deimosOffset);
	goDeimos.GetTransform().SetWorldScale(Vector2::One * deimosScale);
	goDeimos.SetParent(goDeimosPivot);
	goDeimos.SetTexture(GameEngine::DeimosTexture);

	// Phobos
	GameObject& goPhobosPivot = CreateNewGameObject(GameEngine::PhobosPivotGo);
	goPhobosPivot.GetTransform().SetWorldPosition(goMars.GetTransform().GetWorldPosition());
	goPhobosPivot.SetParent(goMars);

	GameObject& goPhobos = CreateNewGameObject(GameEngine::PhobosGo);
	goPhobos.SetMesh(GameEngine::QuadMesh);
	goPhobos.GetTransform().SetWorldPosition(phobosOffset);
	goPhobos.GetTransform().SetWorldScale(Vector2::One * phobosScale);
	goPhobos.SetParent(goPhobosPivot);
	goPhobos.SetTexture(GameEngine::PhobosTexture);

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
