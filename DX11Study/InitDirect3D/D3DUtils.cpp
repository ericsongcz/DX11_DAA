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
	va_list args;			//定义一个va_list类型的变量，用来储存单个参数  
	va_start(args, foramt);	//使args指向可变参数的第一个参数  
	vprintf(foramt, args);  //必须用vprintf等带V的  
	va_end(args);			//结束可变参数的获取  
}

void DisplayVector(float x, float y, float z)
{
	Log("(%f, %f, %f)\n", x, y, z);
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