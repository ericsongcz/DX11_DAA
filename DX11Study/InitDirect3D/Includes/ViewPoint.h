#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class ViewPoint
{
public:
	ViewPoint();
	~ViewPoint();

	void setPosition(float x, float y, float z);
	void setPosition(const FXMVECTOR& position);
	void lookAt(float x, float y, float z);
	void lookAt(const FXMVECTOR& lookAt);
	void setProjectionParameters(float fov, float aspectRatio, float nearPlane, float farPlane);
	void setOrthogonalParameters(float width, float nearPlane, float farPlane);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjectionMatrix();
	XMMATRIX getOrthogonalMatrix();
private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mLookAt;
	XMFLOAT4X4 mViewMatrx;
	XMFLOAT4X4 mProjectionMatrix;
	XMFLOAT4X4 mOrthogonalMatrix;
	float mFOV;
	float mAspectRatio;
	float mNearPlane;
	float mFarPlane;
	float mWidth;
};