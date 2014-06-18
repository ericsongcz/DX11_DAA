cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};

cbuffer LightBuffer : register(b1)
{
	float4 lightDirection;
}

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplerState : register(s0)
{
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	float4 normal = normalTexture.Sample(samplerState, input.texcoord);

	float4 lightDir = -lightDirection;

	float4 ambientColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 diffuseColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float lightIntensity = saturate(dot(normal, lightDir));

	float4 outputColor = (ambientColor + diffuseColor * lightIntensity) * textureColor;
	return outputColor;
}