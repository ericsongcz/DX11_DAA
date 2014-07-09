#pragma once

#include <DirectXMath.h>
#include "ViewPoint.h"

using namespace DirectX;

class Light
{
public:
	enum ELightType
	{
		LT_Point,
		LT_Dirctional,
		LT_Spot
	};

	Light();
	~Light();

	void setPosition(float x, float y, float z);
	void setPosition(const FXMVECTOR& position);
	XMVECTOR getPosition() const;
	void setDirection(float x, float y, float z);
	void setDirection(const FXMVECTOR& direction);
	XMVECTOR getDirection() const;
	void lookAt(float x, float y, float z);
	void lookAt(const FXMVECTOR& lookAt);
	void setProjectionParameters(float fov, float aspectRatio, float nearPlane, float farPlane);
	void setOrthogonalParameters(float width, float nearPlane, float farPlane);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjectionMatrix();
	XMMATRIX getOrthogonalMatrix();
private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mDirection;
	ViewPoint mViewPoint;
};