#pragma once
#include <d3d11.h>
#include "D3DUtils.h"

enum ESamplerType
{
	ST_POINT,
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
	XMFLOAT4X4 viewMatrix2;
	XMFLOAT4X4 projectionMatrix2;
	XMFLOAT4X4 lightViewMatrix;
	XMFLOAT4X4 lightProjectionMatrix;
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
	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
};

struct CommonBuffer
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	int showDepthComplexity;
	int showShadow;
	int commonBufferPad2;
	int commonBufferPad3;
};

struct ReflectionBuffer
{
	XMFLOAT4X4 reflectionMatrix;
};

struct FogBuffer
{
	XMFLOAT4 fogColor;
	float fogStart;
	float fogRange;
	float fogDensity;
	int fogType;
	int showFog;
	int fogBufferPad1;
	int fogBufferPad2;
	int fogBufferPad3;
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