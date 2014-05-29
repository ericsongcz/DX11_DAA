#include "stdafx.h"
#include "D3DUtils.h"
#include <cstdio>
#include <cstdarg>

float RGB256(UINT c)
{
	return 1.0f / 256.0f * c;
}

void Log(const char* foramt, ...)
{
	va_list args;			//����һ��va_list���͵ı������������浥������  
	va_start(args, foramt);	//ʹargsָ��ɱ�����ĵ�һ������  
	vprintf(foramt, args);  //������vprintf�ȴ�V��  
	va_end(args);			//�����ɱ�����Ļ�ȡ  
}

void DisplayVector(float x, float y, float z)
{
	Log("(%f, %f, %f)\n", x, y, z);
}

void XMFLOAT3Negative(XMFLOAT3& out, XMFLOAT3& in)
{
	out.x = -in.x;
	out.y = -in.y;
	out.z = -in.z;
}

void XMFLOAT3Add(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	XMVECTOR temp1 = XMLoadFloat3(&lhs);
	XMVECTOR temp2 = XMLoadFloat3(&rhs);

	XMVECTOR result = XMVectorAdd(temp1, temp2);

	XMStoreFloat3(&out, result);
}

void XMFLOAT3Sub(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	XMVECTOR temp1 = XMLoadFloat3(&lhs);
	XMVECTOR temp2 = XMLoadFloat3(&rhs);

	XMVECTOR result = XMVectorSubtract(temp1, temp2);

	XMStoreFloat3(&out, result);
}

void XMFLOAT3Cross(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	XMVECTOR temp1 = XMLoadFloat3(&lhs);
	XMVECTOR temp2 = XMLoadFloat3(&rhs);

	XMVECTOR result = XMVector3Cross(temp1, temp2);

	XMStoreFloat3(&out, result);
}

void XMFLOAT3Normalize(XMFLOAT3& out, XMFLOAT3& in)
{
	XMVECTOR temp = XMLoadFloat3(&in);

	XMVECTOR result = XMVector3Normalize(temp);

	XMStoreFloat3(&out, result);
}

bool XMFLOAT3Equal(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	if ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z))
	{
		return true;
	}

	return false;
}