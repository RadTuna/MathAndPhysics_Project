#pragma once

namespace CK
{
namespace DD
{ 

class TransformComponent
{
public:
	TransformComponent() = default;
	TransformComponent(const Transform2D& InLocalTransform) : _LocalTransform(InLocalTransform) 
	{ 
		UpdateWorld(); 
	}

public: // 로컬 트랜스폼 관련 함수
	FORCEINLINE Transform2D& GetLocalTransform() { return _LocalTransform; }
	FORCEINLINE void SetLocalTransform(const Transform2D& InTransform) { _LocalTransform = InTransform; UpdateWorld(); }
	FORCEINLINE const Transform2D& GetLocalTransform() const { return _LocalTransform; }

	FORCEINLINE void SetLocalPosition(const Vector2& InPosition);
	FORCEINLINE void AddLocalPosition(const Vector2& InDeltaPosition);
	FORCEINLINE void AddLocalRotation(float InDegree);
	FORCEINLINE void SetLocalRotation(float InDegree);
	FORCEINLINE void SetLocalScale(const Vector2& InScale);

	FORCEINLINE Vector2 GetLocalPosition() const { return _LocalTransform.GetPosition(); }
	FORCEINLINE float GetLocalRotation() const { return _LocalTransform.GetRotation(); }
	FORCEINLINE Vector2 GetLocalScale() const { return _LocalTransform.GetScale(); }

	FORCEINLINE Vector2 GetLocalX() const { return _LocalTransform.GetXAxis(); }
	FORCEINLINE Vector2 GetLocalY() const { return _LocalTransform.GetYAxis(); }
	FORCEINLINE Matrix3x3 GetLocalMatrix() const { return _LocalTransform.GetMatrix(); }

public: // 월드 트랜스폼 관련 함수
	FORCEINLINE Transform2D& GetWorldTransform() { return _WorldTransform; }
	FORCEINLINE void SetWorldTransform(const Transform2D& InTransform) { _WorldTransform = InTransform; UpdateLocal(); }
	FORCEINLINE const Transform2D& GetWorldTransform() const { return _WorldTransform; }

	FORCEINLINE void SetWorldPosition(const Vector2& InPosition);
	FORCEINLINE void AddWorldPosition(const Vector2& InDeltaPosition);
	FORCEINLINE void AddWorldRotation(float InDegree);
	FORCEINLINE void SetWorldRotation(float InDegree);
	FORCEINLINE void SetWorldScale(const Vector2& InScale);

	FORCEINLINE Vector2 GetWorldPosition() const { return _WorldTransform.GetPosition(); }
	FORCEINLINE float GetWorldRotation() const { return _WorldTransform.GetRotation(); }
	FORCEINLINE Vector2 GetWorldScale() const { return _WorldTransform.GetScale(); }

	FORCEINLINE Matrix3x3 GetWorldMatrix() const { return _WorldTransform.GetMatrix(); }

public: // 계층 구조 관련 함수
	bool SetRoot();
	bool RemoveFromParent();
	TransformComponent& GetRoot();
	bool SetParent(TransformComponent& InTransform);

	FORCEINLINE bool HasParent() const { return _ParentPtr != nullptr; }
	std::vector<TransformComponent*>& GetChildren() { return _ChildrenPtr; }
	std::vector<TransformComponent*>::const_iterator ChildBegin() const { return _ChildrenPtr.begin(); }
	std::vector<TransformComponent*>::const_iterator ChildEnd() const { return _ChildrenPtr.end(); }

private: // 내부에서만 호출하는 함수
	FORCEINLINE TransformComponent* GetParentPtr() const { return _ParentPtr; }
	void UpdateLocal();
	void UpdateWorld();
	void UpdateChildrenWorld();

private: // 계층 구조를 위한 변수
	Transform2D _LocalTransform;
	Transform2D _WorldTransform;

	TransformComponent* _ParentPtr = nullptr;
	std::vector<TransformComponent*> _ChildrenPtr;
};


FORCEINLINE void TransformComponent::SetLocalPosition(const Vector2& InPosition)
{
	_LocalTransform.SetPosition(InPosition);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalPosition(const Vector2& InDeltaPosition)
{
	_LocalTransform.AddPosition(InDeltaPosition);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::AddLocalRotation(float InDegree)
{
	_LocalTransform.AddRotation(InDegree);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalRotation(float InDegree)
{
	_LocalTransform.SetRotation(InDegree);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetLocalScale(const Vector2& InScale)
{
	_LocalTransform.SetScale(InScale);
	UpdateWorld();
}

FORCEINLINE void TransformComponent::SetWorldPosition(const Vector2& InPosition)
{
	_WorldTransform.SetPosition(InPosition);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldPosition(const Vector2& InDeltaPosition)
{
	_WorldTransform.AddPosition(InDeltaPosition);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::AddWorldRotation(float InDegree)
{
	_WorldTransform.AddRotation(InDegree);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldRotation(float InDegree)
{
	_WorldTransform.SetRotation(InDegree);
	UpdateLocal();
}

FORCEINLINE void TransformComponent::SetWorldScale(const Vector2& InScale)
{
	_WorldTransform.SetScale(InScale);
	UpdateLocal();
}
}
}