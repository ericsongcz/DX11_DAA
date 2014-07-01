#pragma once
#include <d3d11.h>

class SharedD3DDevice
{
public:
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
};