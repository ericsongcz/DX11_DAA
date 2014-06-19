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
};

Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);

SamplerState samplerState : register(s0)
{
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 position = positionTexture.Sample(samplerState, input.texcoord);

	// Point light.
	//float3 lightDir = normalize(lightPosition - position).xyz;

	// Directional light.
	float4 lightDir = -lightDirection;

	float4 normal = normalTexture.Sample(samplerState, input.texcoord);

	float diffuse = saturate(dot(normal, lightDir));

	// All color components are summed in the pixel shader.
	float4 outputColor;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	outputColor = (ambientColor * ambientIntensity + diffuseColor * diffuse * diffuseIntensity) * textureColor/* + specular * 0.5f*/;

	return outputColor;
}