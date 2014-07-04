#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class ViewPoint
{
public:
	ViewPoint();
	~ViewPoint();

	void setPosition(float x, float y, float z);
	void lookAt(float x, float y, float z);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mLookAt;
	XMFLOAT4X4 mViewMatrx;
	XMFLOAT4X4 mProjectionMatrix;
	float mFOV;
	float mAspectRatio;
	float mNearPlane;
	float mFarPlane;
};