#pragma once

namespace CK
{
namespace DD
{

class CameraObject
{
public:
	CameraObject() = default;
	~CameraObject() { }

public:
	// Ʈ������
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }

	// ī�޶� ���� �������� �Լ�
	const ScreenPoint& GetViewportSize() const { return _ViewportSize; }

	// ī�޶� ���� �����ϴ� �Լ�
	void SetViewportSize(const ScreenPoint & InViewportSize) { _ViewportSize = InViewportSize; }

	// ��� ����
	FORCEINLINE Matrix3x3 GetViewMatrix() const;

private:
	TransformComponent _Transform;
	ScreenPoint _ViewportSize;
};

FORCEINLINE Matrix3x3 CameraObject::GetViewMatrix() const
{
	Matrix3x3 viewMatrix = Matrix3x3(
		Vector3(_Transform.GetLocalX(), 0.f), 
		Vector3(_Transform.GetLocalY(), 0.f), 
		Vector3::UnitZ).Tranpose();

	viewMatrix.Cols[2] = Vector3(
		_Transform.GetLocalX().Dot(-_Transform.GetPosition()),
		_Transform.GetLocalY().Dot(-_Transform.GetPosition()),
		1.f);

	return viewMatrix;
}

}
}