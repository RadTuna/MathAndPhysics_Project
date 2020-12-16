#pragma once

namespace CK
{
namespace DD
{

class Bone
{
public:
	Bone() = default;
	Bone(const std::string& InName, const Transform2D& InTransform) : _Name(InName), _ParentBoneName(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
		_BindPose = InTransform;
		_Transform.SetLocalTransform(InTransform);
	}

public:
	// 트랜스폼
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }
	const Transform2D& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone)
	{
	    _ParentBoneName = InBone.GetName();
		_Transform.SetParent(InBone.GetTransform());
	}

	// 키 관련
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }
	bool HasParent() const { return _Name.compare(_ParentBoneName) != 0 && !_ParentBoneName.empty(); }
	const std::string& GetParentName() const { return _ParentBoneName; }

private:
	std::size_t _Hash = 0;

	// 본의 이름
	std::string _Name;

	// 본의 현재 트랜스폼 정보
	TransformComponent _Transform;

	// 본의 최초 트랜스폼 정보
	Transform2D _BindPose;

	std::string _ParentBoneName;

private: // 계층 구조 변수
	Bone* _ParentPtr = nullptr;
	std::vector<Bone*> _ChildrenPtr;

};

}
}