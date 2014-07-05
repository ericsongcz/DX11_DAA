#include "stdafx.h"
#include "ViewPoint.h"

ViewPoint::ViewPoint()
{
}

ViewPoint::~ViewPoint()
{
}

void ViewPoint::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void ViewPoint::lookAt(float x, float y, float z)
{
	mLookAt.x = -x;
	mLookAt.y = -y;
	mLookAt.z = -z;
}

void ViewPoint::setProjectionParameters(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	mFOV = fov;
	mAspectRatio = aspectRatio;
	mNearPlane = nearPlane;
	mFarPlane = farPlane;
}


XMMATRIX ViewPoint::getViewMatrix()
{
	XMVECTOR eye = XMLoadFloat3(&mPosition);
	XMVECTOR lookAt = XMLoadFloat3(&mLookAt);
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMVECTOR right;

	// ����view�ֲ�����ϵ����ı˴�������
	lookAt = XMVector3Normalize(lookAt);

	right = XMVector3Cross(up, lookAt);
	right = XMVector3Normalize(right);

	// look x right
	up = XMVector3Cross(lookAt, right);
	up = XMVector3Normalize(up);

	// �������XMMatrixLookAtLH�����ɳ���ʼ��Ϊ�۲�����ͼ����
	XMMATRIX viewMatrix;
	//viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);

	// ���������ֶ�����һ����ͼ�������������ʵ��ƽ�ơ�
	XMVECTOR row1 = XMVectorSet(XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(lookAt), 0.0f);
	XMVECTOR row2 = XMVectorSet(XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(lookAt), 0.0f);
	XMVECTOR row3 = XMVectorSet(XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(lookAt), 0.0f);

	// ��eyeת����view space��
	float x = -XMVectorGetX(XMVector3Dot(eye, right));
	float y = -XMVectorGetX(XMVector3Dot(eye, up));
	float z = -XMVectorGetX(XMVector3Dot(eye, lookAt));

	XMVECTOR row4 = XMVectorSet(x, y, z, 1.0f);

	viewMatrix = XMMATRIX(row1, row2, row3, row4);

	return viewMatrix;
}

XMMATRIX ViewPoint::getProjectionMatrix()
{
	return XMMatrixPerspectiveFovRH(mFOV, mAspectRatio, mNearPlane, mFarPlane);
}