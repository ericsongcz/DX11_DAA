#include "LightHelper.hlsli"

cbuffer LightBuffer : register(b0)
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
	float4x4 worldMatrix;
};

Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D tangentTexture : register(t3);

SamplerState samplerState : register(s0)
{
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 worldPosition = positionTexture.Sample(samplerState, input.texcoord);
	float4 normal = normalTexture.Sample(samplerState, input.texcoord);
	float4 tangent = tangentTexture.Sample(samplerState, input.texcoord);

	worldPosition.w = 1.0f;
	normal.w = 0.0f;
	tangent.w = 0.0f;

	// Point light.
	float3 lightDir = normalize(lightPosition - worldPosition).xyz;

	// Directional light.
	//float3 lightDir = -normalize(lightDirection);

	float diffuse = saturate(dot(normal.xyz, lightDir));

	// All color components are summed in the pixel shader.
	float4 outputColor;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	outputColor = (ambientColor * ambientIntensity + diffuseColor * diffuse * diffuseIntensity) * textureColor/* + specular * 0.5f*/;

	return outputColor;
}