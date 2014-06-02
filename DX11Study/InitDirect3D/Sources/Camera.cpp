#include "stdafx.h"
#include "Camera.h"
#include "D3DUtils.h"

Camera::Camera()
{
	XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());
#if USE_RIGHT_HAND
	mPosition = XMFLOAT3(0.0f, 0.0f, 10.0f);
#else
	mPosition = XMFLOAT3(0.0f, 0.0f, -10.0f);
#endif
	mLookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mRight = XMFLOAT3(1.0f, 0.0f, 0.0f);

	mCameraType = AIRCRAFT;
}

Camera::~Camera()
{
}

void Camera::setWorldMatrix(const CXMMATRIX& worldMatrix)
{
	XMStoreFloat4x4(&mWorldMatrix, worldMatrix);
}

XMMATRIX Camera::getWolrdMatrix() const
{
	return XMLoadFloat4x4(&mWorldMatrix);
}

XMMATRIX Camera::getOrthogonalWorldMatrix() const
{
	XMMATRIX worldMatrx = XMMatrixIdentity();

	worldMatrx.r[3] = XMVectorSet(-400.0f, 300.0f, 0.0f, 1.0f);

	return worldMatrx;
}

XMMATRIX Camera::getViewMatrix() const
{
	XMVECTOR eye = XMLoadFloat3(&mPosition);
	XMVECTOR lookAt = XMLoadFloat3(&mLookAt);
	XMVECTOR up = XMLoadFloat3(&mUp);
	XMVECTOR right = XMLoadFloat3(&mRight);

	// 保持view局部坐标系各轴的彼此正交。
	lookAt = XMVector3Normalize(lookAt);

	// look x right
	up = XMVector3Cross(lookAt, right);
	up = XMVector3Normalize(up);

	right = XMVector3Cross(up, lookAt);
	right = XMVector3Normalize(right);


	// 理解有误，XMMatrixLookAtLH是生成朝向始终为观察点的视图矩阵。
	XMMATRIX viewMatrix;
	//viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);

	// 所以我们手动创建一个视图矩阵，让相机可以实现平移。
	XMVECTOR row1 = XMVectorSet(XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(lookAt), 0.0f);
	XMVECTOR row2 = XMVectorSet(XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(lookAt), 0.0f);
	XMVECTOR row3 = XMVectorSet(XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(lookAt), 0.0f);

	// 将eye转换到view space。
	float x = -XMVectorGetX(XMVector3Dot(right, eye));
	float y = -XMVectorGetX(XMVector3Dot(up, eye));
	float z = -XMVectorGetX(XMVector3Dot(lookAt, eye));

	XMVECTOR row4 = XMVectorSet(x, y, z, 1.0f);

	viewMatrix = XMMATRIX(row1, row2, row3, row4);

#if USE_RIGHT_HAND
	viewMatrix = XMMatrixLookAtRH(eye, lookAt, up);
#else
	//viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);
#endif

	return viewMatrix;
}

XMMATRIX Camera::getProjectionMatrix() const
{
	float fov = XM_PI / 4;

	// 创建透视投影矩阵。
#if USE_RIGHT_HAND
	return XMMatrixPerspectiveFovRH(fov, mAspectRatio, 1.0f, 1000.0f);
#else
	return XMMatrixPerspectiveFovLH(fov, mAspectRatio, 1.0f, 1000.0f);
#endif
}

XMMATRIX Camera::getOrthogonalMatrix() const
{
	return XMMatrixOrthographicLH(800, 600, 1.0f, 1000.0f);
}

void Camera::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
}

void Camera::setPosition(FXMVECTOR position)
{
	XMStoreFloat3(&mPosition, position);
}

void Camera::lookAt(FXMVECTOR lookAt)
{
	XMStoreFloat3(&mLookAt, lookAt);
}

void Camera::strafe(float units)
{
	// 仅在x，z平面移动
	XMVECTOR position = XMLoadFloat3(&mPosition);
	if (mCameraType == LANDOBJECT)
	{
		XMVECTOR limitAxis = XMVectorSet(mRight.x, 0.0f, mRight.z, 0.0f);

		position += limitAxis * units;
	}
	else
	{
		XMVECTOR right = XMLoadFloat3(&mRight);
		position += right * units;
	}

	XMStoreFloat3(&mPosition, position);
}

void Camera::fly(float units)
{
	// 仅在y轴移动。
	XMVECTOR position = XMLoadFloat3(&mPosition);
	if (mCameraType == LANDOBJECT)
	{
		mPosition.y += units;
	}
	else
	{
		XMVECTOR up = XMLoadFloat3(&mUp);
		position += up * units;
		XMStoreFloat3(&mPosition, position);
	}
}

void Camera::walk(float units)
{
	// 仅在x，z平面移动
	XMVECTOR position = XMLoadFloat3(&mPosition);
	if (mCameraType == LANDOBJECT)
	{
		XMVECTOR limitAxis = XMVectorSet(mLookAt.x, 0.0f, mLookAt.z, 0.0f);

		position += limitAxis * units;
	}
	else
	{
		XMVECTOR lookAt = XMLoadFloat3(&mLookAt);
		position += lookAt * units;
	}

	XMStoreFloat3(&mPosition, position);
}

void Camera::pitch(float angle)
{
	XMMATRIX rotate;
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR up = XMLoadFloat3(&mUp);
	XMVECTOR lookAt = XMLoadFloat3(&mLookAt);
	rotate = XMMatrixRotationAxis(right, angle);

	// 绕right向量，旋转up和look。
	up = XMVector3TransformCoord(up, rotate);
	XMStoreFloat3(&mUp, up);

	lookAt = XMVector3TransformCoord(lookAt, rotate);
	XMStoreFloat3(&mLookAt, lookAt);
}

void Camera::yaw(float angle)
{
	XMMATRIX rotate;
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR up = XMLoadFloat3(&mUp);
	XMVECTOR lookAt = XMLoadFloat3(&mLookAt);

	// 对于LANDOBJECT总是绕(0, 1, 0)旋转。
	if (mCameraType == LANDOBJECT)
	{
		rotate = XMMatrixRotationY(angle);
	}
	// 对于AIRCRAFT，绕着up向量旋转。
	else
	{
		XMVECTOR up = XMLoadFloat3(&mUp);
		rotate = XMMatrixRotationAxis(up, angle);

		// 绕up向量，旋转right和look。
		right = XMVector3TransformCoord(right, rotate);
		XMStoreFloat3(&mRight, right);

		lookAt = XMVector3TransformCoord(lookAt, rotate);
		XMStoreFloat3(&mLookAt, lookAt);
	}
}

void Camera::roll(float angle)
{
	// 只对AIRCRAFT模式才作roll旋转。
	if (mCameraType == AIRCRAFT)
	{
		XMVECTOR right = XMLoadFloat3(&mRight);
		XMVECTOR up = XMLoadFloat3(&mUp);
		XMVECTOR lookAt = XMLoadFloat3(&mLookAt);
		XMMATRIX rotate = XMMatrixRotationAxis(lookAt, angle);

		// 绕着look向量，旋转up和right。
		up = XMVector3TransformCoord(up, rotate);
		XMStoreFloat3(&mUp, up);

		right = XMVector3TransformCoord(right, rotate);
		XMStoreFloat3(&mRight, right);
	}
}