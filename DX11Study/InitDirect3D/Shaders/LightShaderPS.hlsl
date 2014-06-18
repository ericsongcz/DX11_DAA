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
}

Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);

SamplerState samplerState : register(s0)
{
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
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

	//float3 lightDir = normalize(lightPosition - position).xyz;
	float3 lightDir = -lightDirection.xyz;

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);
	textureColor.w = 1.0f;

	float3 normal = normalTexture.Sample(samplerState, input.texcoord).xyz;

	float4 ambientColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 diffuseColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float lightIntensity = saturate(dot(normal, lightDir));

	float4 outputColor = (ambientColor + diffuseColor * lightIntensity) * textureColor;

	return outputColor;
}