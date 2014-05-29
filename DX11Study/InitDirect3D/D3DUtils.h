//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------
#pragma once
#include <DirectXMath.h>
#include <DxErr.h>

#define RAND_ONE_FLOAT() rand() / (float)(RAND_MAX+1)

using namespace DirectX;

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)\
{\
	HRESULT hr = (x);\
	if(FAILED(hr))\
	{\
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
	}\
}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

// 虚拟键值。
const DWORD VK_A = 0x41;
const DWORD VK_D = 0x44;
const DWORD VK_E = 0x45;
const DWORD VK_F = 0x46;
const DWORD VK_Q = 0x51;
const DWORD VK_S = 0x53;
const DWORD VK_W = 0x57;

//定义一些常用颜色
namespace Colors
{
	const XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	const XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f }; //蓝绿色 
	const XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f }; //洋红色

	const XMVECTORF32 Beach_Sand = { 1.0f, 0.96f, 0.62f, 1.0f };
	const XMVECTORF32 Light_Yellow_Green = { 0.48f, 0.77f, 0.46f, 1.0f };
	const XMVECTORF32 Dark_Yellow_Green = { 0.1f, 0.48f, 0.19f, 1.0f };
	const XMVECTORF32 DarkBrown = { 0.45f, 0.39f, 0.34f, 1.0f };
	const XMVECTORF32 Corn_Flower = { 0.39f, 0.58f, 0.93f, 1.0f };
}

float RGB256(UINT c);

template<typename T>
void SafeRelease(T type)
{
	if (type != nullptr)
	{
		type->Release();
	}
}

template<typename T>
void SafeDelete(T type)
{
	if (type != nullptr)
	{
		delete type;
	}
}

void Log(const char* foramt, ...);
void DisplayVector(float x, float y, float z);

void XMFLOAT3Negative(XMFLOAT3& out, XMFLOAT3& in);
void XMFLOAT3Add(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs);
void XMFLOAT3Sub(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs);
void XMFLOAT3Cross(XMFLOAT3& out, XMFLOAT3& lhs, XMFLOAT3& rhs);
void XMFLOAT3Normalize(XMFLOAT3& out, XMFLOAT3& in);
bool XMFLOAT3Equal(const XMFLOAT3& lhs, const XMFLOAT3& rhs);