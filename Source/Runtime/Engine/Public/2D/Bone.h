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
	// Ʈ������
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }
	const Transform2D& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone)
	{
	    _ParentBoneName = InBone.GetName();
		_Transform.SetParent(InBone.GetTransform());
	}

	// Ű ����
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }
	bool HasParent() const { return _Name.compare(_ParentBoneName) != 0 && !_ParentBoneName.empty(); }
	const std::string& GetParentName() const { return _ParentBoneName; }

private:
	std::size_t _Hash = 0;

	// ���� �̸�
	std::string _Name;

	// ���� ���� Ʈ������ ����
	TransformComponent _Transform;

	// ���� ���� Ʈ������ ����
	Transform2D _BindPose;

	std::string _ParentBoneName;

private: // ���� ���� ����
	Bone* _ParentPtr = nullptr;
	std::vector<Bone*> _ChildrenPtr;

};

}
}