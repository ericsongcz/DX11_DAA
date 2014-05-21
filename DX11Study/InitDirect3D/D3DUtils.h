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