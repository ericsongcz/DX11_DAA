#include <DxErr.h>

#if defined(DEBUG) | defined(_DEBUG) 
#ifndef HR
#define HR(x) \
{\
	HRESULT hr = (x); \
if (FAILED(hr)) \
{\
	DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \ }\
}
#endif
#else
#ifndef HR 
#define HR(x) (x) 
#endif
#endif

float RGB256(UINT c)
{
	return 1.0f / 256.0f * c;
}

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
	delete type;
}