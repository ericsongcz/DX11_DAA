#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	enum ECameraType
	{
		AIRCRAFT,
		LANDOBJECT
	};

	Camera();
	~Camera();

	void setWorldMatrix(const CXMMATRIX& worldMatrix);
	XMMATRIX getWolrdMatrix() const;
	XMMATRIX getOrthogonalWorldMatrix() const;
	XMMATRIX getViewMatrix() const;
	XMMATRIX getProjectionMatrix() const;
	XMMATRIX getOrthogonalMatrix() const;
	void setAspectRatio(float aspectRatio);
	void setPosition(FXMVECTOR position);
	void lookAt(FXMVECTOR lookAt);
	void strafe(float units); // ���ҡ�
	void fly(float units);	  // ���¡�
	void walk(float units);	  // ǰ��
	void pitch(float angle);  // ��תright������
	void yaw(float angle);	  // ��תup������
	void roll(float angle);	  // ��תlook������

private:
	float mAspectRatio;

	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjectionMatrix;
	XMFLOAT3 mPosition;
	XMFLOAT3 mLookAt;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;

	ECameraType mCameraType;
};