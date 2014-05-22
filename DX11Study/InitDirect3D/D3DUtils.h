//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------
#pragma once

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

// Colors namespace defined in d3dUtil.h.
//
// #define XMGLOBALCONST extern CONST __declspec(selectany)
// 1. extern so there is only one copy of the variable, and not a // separate private copy in each .obj.
// 2. __declspec(selectany) so that the compiler does not complain // about multiple definitions in a .cpp file (it can pick anyone // and discard the rest because they are constant--all the same). namespace Colors
XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f }; 
XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

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