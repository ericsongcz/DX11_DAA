#include "LightHelper.hlsli"

cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
	float4x4 textureTransformMatrix;
	float4x4 viewMatrix2;
	float4x4 projectionMatrix2;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 lightViewPosition : TEXCOORD1;
	float3 normal : NORMAL;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	input.position.w = 1.0f;
	input.normal.w = 0.0f;

	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjectionMatrix);
	//output.position = mul(input.position, worldMatrix);
	//output.position = mul(output.position, lightViewMatrix);
	//output.position = mul(output.position, lightProjectionMatrix);

	// Calculate the position of the vertices as viewed by the light source.
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	output.normal = normalize(mul(input.normal, worldMatrix).xyz);

	return output;
}