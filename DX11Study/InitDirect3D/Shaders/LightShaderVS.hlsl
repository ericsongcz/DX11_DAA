#include "LightHelper.hlsli"

cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};

cbuffer LightBuffer : register(b1)
{
	float4 lightPosition;
	float4 lightDirection;
	float4 ambientColor;
	float4 diffuseColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	float4 cameraPosition;
	float4 specularColor;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.texcoord = input.texcoord;

	return output;
}