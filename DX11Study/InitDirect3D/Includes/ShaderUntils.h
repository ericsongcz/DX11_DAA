#pragma once
#include <d3d11.h>
#include "D3DUtils.h"

enum ESamplerType
{
	ST_LINEAR,
	ST_ANISOTROPIC
};

struct DirectionalLight
{
	XMFLOAT4 diffuseColor;
	XMFLOAT4 direction;
};

struct PointLight
{
	XMFLOAT4 diffuseColor;
	XMFLOAT4 position;
	float radius;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};

struct Spotlight
{
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	float radius;
	XMFLOAT3 direction;
	float spot;
};

struct MatrixBuffer
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 worldViewProjectionMatrix;
	XMFLOAT4X4 textureTransformMatrix;
};

struct LightBuffer
{
	XMFLOAT4 ambientColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	XMFLOAT4 cameraPositon;
	XMFLOAT4 specularColor;
	DirectionalLight directionalLight;
	PointLight pointLight;
	Spotlight spotLight;
};

struct CommonBuffer
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
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