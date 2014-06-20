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
	XMMATRIX getBaseViewMatrix() const;
	XMMATRIX getViewMatrix();
	XMMATRIX getInverseViewMatrix() const;
	XMMATRIX getProjectionMatrix() const;
	XMMATRIX getOrthogonalMatrix() const;
	void setAspectRatio(float aspectRatio);
	void setOrthogonalMatrix(float width, float height, float nearZ = 1.0f, float farZ = 1000.0f);
	void setPosition(FXMVECTOR position);
	XMFLOAT3 getPosition() const;
	void lookAt(FXMVECTOR lookAt);
	void strafe(float units); // 左右。
	void fly(float units);	  // 上下。
	void walk(float units);	  // 前后。
	void pitch(float angle);  // 旋转right向量。
	void yaw(float angle);	  // 旋转up向量。
	void roll(float angle);	  // 旋转look向量。

private:
	float mAspectRatio;

	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mBaseViewMatrix;
	XMMATRIX mOrthogonalMatrix;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mInverViewMatrix;
	XMFLOAT4X4 mProjectionMatrix;
	XMFLOAT3 mPosition;
	XMFLOAT3 mLookAt;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;

	ECameraType mCameraType;
};