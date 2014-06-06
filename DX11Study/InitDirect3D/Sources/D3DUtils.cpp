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
#if LOG_ON
	va_list args;			//定义一个va_list类型的变量，用来储存单个参数  
	va_start(args, foramt);	//使args指向可变参数的第一个参数  
	vprintf(foramt, args);  //必须用vprintf等带V的  
	va_end(args);			//结束可变参数的获取  
#endif
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

void XMFLOAT3Add(XMFLOAT3& out, const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	XMVECTOR temp1 = XMLoadFloat3(&lhs);
	XMVECTOR temp2 = XMLoadFloat3(&rhs);

	XMVECTOR result = XMVectorAdd(temp1, temp2);

	XMStoreFloat3(&out, result);
}

void XMFLOAT3Sub(XMFLOAT3& out, const XMFLOAT3& lhs, const XMFLOAT3& rhs)
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

bool XMFLOAT2Equal(const XMFLOAT2& lhs, const XMFLOAT2& rhs)
{
	if ((lhs.x == rhs.x) && (lhs.y == rhs.y))
	{
		return true;
	}

	return false;
}

XMMATRIX RotationX(float angle)
{
	return XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), angle);
}

XMMATRIX RotationY(float angle)
{
	return XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), angle);
}

XMMATRIX RotationZ(float angle)
{
	return XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f)), angle);
}

ID3D11ShaderResourceView* CreateShaderResourceViewFromFile(const string& fileName, ID3D11Device* device)
{
	TexMetadata metaData;
	ScratchImage image;

	if (fileName.size() > 0)
	{
		size_t len = fileName.size() + 1;

		size_t converted = 0;

		wchar_t *WStr;

		WStr = (wchar_t*)malloc(len*sizeof(wchar_t));

		mbstowcs_s(&converted, WStr, len, fileName.c_str(), _TRUNCATE);

		HR(LoadFromDDSFile(WStr, DDS_FLAGS_NONE, &metaData, image));

		ID3D11ShaderResourceView* shaderResourceView = nullptr;
		HR(CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metaData, &shaderResourceView));

		return shaderResourceView;
	}

	return nullptr;
}