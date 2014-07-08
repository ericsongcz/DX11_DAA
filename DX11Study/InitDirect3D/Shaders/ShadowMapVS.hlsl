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

cbuffer LightBuffer : register(b1)
{
	float4 ambientColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	float4 cameraPosition;
	float4 specularColor;
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

cbuffer CommonBuffer : register(b2)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	int showDepthComplexity;
	int showShadow;
	int commonBufferPad2;
	int commonBufferPad3;
}

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texcoord : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 lightViewPosition : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 texcoord : TEXCOORD0;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	input.position.w = 1.0f;
	input.normal.w = 0.0f;
	input.tangent.w = 0.0f;

	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjectionMatrix);
	//output.position = mul(input.position, worldMatrix);
	//output.position = mul(output.position, lightViewMatrix);
	//output.position = mul(output.position, lightProjectionMatrix);

	// Calculate the position of the vertices as viewed by the light source.
	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	float3 tangentW = mul(input.tangent, worldMatrix).xyz;

	float3 N = normalize(mul(input.normal, worldMatrix)).xyz;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	//float3 B = normalize(mul(input.binormal, worldMatrix)).xyz;
	float3 B = cross(T, N);

	output.normal = N;
	output.tangent = T;
	output.binormal = B;
	
	output.texcoord = input.texcoord;

	return output;
}