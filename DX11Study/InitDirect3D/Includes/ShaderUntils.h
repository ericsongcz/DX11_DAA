#pragma once
#include <d3d11.h>
#include "D3DUtils.h"

enum ESamplerType
{
	ST_LINEAR,
	ST_ANISOTROPIC
};

struct PointLight
{
	XMFLOAT4 diffuseColor;
	XMFLOAT4 position;
	float range;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};

struct Spotlight
{
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	float range;
	XMFLOAT3 direction;
	float spot;
};

struct ShaderData
{
	ShaderData()
		: vertexShader(nullptr),
		pixelShader(nullptr),
		vertexShaderSize(0),
		vertexShaderData(nullptr),
		pixelShaderSize(0),
		pixelShaderData(nullptr)
	{}

	~ShaderData()
	{
		SafeDelete(vertexShaderData);
		SafeDelete(pixelShaderData);
	}

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	size_t vertexShaderSize;
	char* vertexShaderData;
	size_t pixelShaderSize;
	char* pixelShaderData;
};

ShaderData* createVertexAndPixelShader(ID3D11Device* device, const wchar_t* vertexShaderFileName, const wchar_t* pixelShaderFileName);