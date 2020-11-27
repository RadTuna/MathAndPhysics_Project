#pragma once

namespace CK
{
namespace DD
{

class GameEngine : public EngineInterface
{
public:
	GameEngine() = default;

public:
	// ���� �������̽�
	virtual bool Init() override;
	virtual bool IsInitialized() override { return _IsInitialized; }
	virtual void OnScreenResize(const ScreenPoint & InScreenSize) override;
	virtual InputManager& GetInputManager() override { return _InputManager; }

	// ���� ���� �뵵 
	const InputManager& GetInputManager() const { return _InputManager; }

	// ���ҽ� ����
	Mesh& CreateMesh(const std::size_t & InKey);
	Texture& CreateTexture(const std::size_t & InKey, const std::string & InTexturePath);

	// ���� ������Ʈ
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	GameObject& CreateNewGameObject(const std::string & InName);
	GameObject& GetGameObject(const std::string & InName);

	// �޽�
	Mesh& GetMesh(const std::size_t & InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t & InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// ī�޶� 
	FORCEINLINE CameraObject& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const CameraObject& GetMainCamera() const { return _MainCamera; }

	// ���� �ؽ���
	FORCEINLINE const Texture& GetTexture(const std::size_t & InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

	// ���� �׸��� ���� ���
	std::unordered_map<std::string, GameObject*> GetBoneObjectPtrs() { return _BoneGameObjectPtrs; }

private:
	bool LoadResources();
	bool LoadScene();

public: // �ֿ� Ű ��
	// �޽�
	static const std::size_t QuadMesh;

	// ���� ������Ʈ
	static const std::string SunGo;
	static const std::string MercuryPivotGo;
	static const std::string MercuryGo;
	static const std::string VenusPivotGo;
	static const std::string VenusGo;
	static const std::string EarthPivotGo;
	static const std::string EarthGo;
	static const std::string MoonPivotGo;
	static const std::string MoonGo;
	static const std::string MarsPivotGo;
	static const std::string MarsGo;
	static const std::string DeimosPivotGo;
	static const std::string DeimosGo;
	static const std::string PhobosPivotGo;
	static const std::string PhobosGo;

	// �ؽ���
	static const std::size_t SunTexture;
	static const std::size_t MercuryTexture;
	static const std::size_t VenusTexture;
	static const std::size_t EarthTexture;
	static const std::size_t MoonTexture;
	static const std::size_t MarsTexture;
	static const std::size_t DeimosTexture;
	static const std::size_t PhobosTexture;

private:
	bool _IsInitialized = false;

	ScreenPoint _ScreenSize;
	InputManager _InputManager;
	CameraObject _MainCamera;

	std::vector<std::unique_ptr<GameObject>> _Scene;
	std::unordered_map<std::size_t, std::unique_ptr<Mesh>> _Meshes;
	std::unordered_map<std::size_t, std::unique_ptr<Texture>> _Textures;
	std::unordered_map<std::string, GameObject*> _BoneGameObjectPtrs;
};

}
}