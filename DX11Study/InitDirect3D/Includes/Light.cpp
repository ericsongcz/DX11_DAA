#include "stdafx.h"
#include "Light.h"

Light::Light()
{
}

Light::~Light()
{
}

void Light::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;

	mViewPoint.setPosition(x, y, z);
}

void Light::setPosition(const FXMVECTOR& position)
{
	XMStoreFloat3(&mPosition, position);

	mViewPoint.setPosition(position);
}

XMVECTOR Light::getPosition() const
{
	return XMLoadFloat3(&mPosition);
}


void Light::setDirection(float x, float y, float z)
{
	mDirection.x = x;
	mDirection.y = y;
	mDirection.z = z;
}

void Light::setDirection(const FXMVECTOR& direction)
{
	XMStoreFloat3(&mDirection, direction);
}

XMVECTOR Light::getDirection() const
{
	return XMLoadFloat3(&mDirection);
}

void Light::lookAt(float x, float y, float z)
{
	mViewPoint.lookAt(x, y, z);
}

void Light::lookAt(const FXMVECTOR& lookAt)
{
	mViewPoint.lookAt(lookAt);
}

void Light::setProjectionParameters(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	mViewPoint.setProjectionParameters(fov, aspectRatio, nearPlane, farPlane);
}

void Light::setOrthogonalParameters(float width, float nearPlane, float farPlane)
{
	mViewPoint.setOrthogonalParameters(width, nearPlane, farPlane);
}

XMMATRIX Light::getViewMatrix()
{
	return mViewPoint.getViewMatrix();
}

XMMATRIX Light::getProjectionMatrix()
{
	return mViewPoint.getProjectionMatrix();
}

XMMATRIX Light::getOrthogonalMatrix()
{
	return mViewPoint.getOrthogonalMatrix();
}
