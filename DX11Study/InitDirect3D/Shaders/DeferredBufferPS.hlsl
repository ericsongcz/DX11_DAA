cbuffer CommonBuffer : register(b0)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
};

Texture2D colorTexture : register(t0);
Texture2D normalMapTexture : register(t1);

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
	float4 worldPosition : POSITION;
	float2 texcoord : TEXCOORD;
	float4 normal : NORMAL;
};

struct PixelOutput
{
	float4 position : SV_TARGET0;
	float4 color : SV_TARGET1;
	float4 normal : SV_TARGET2;
};

PixelOutput main(PixelInput input)
{
	PixelOutput output;

	output.position = input.worldPosition;

	float4 normalWorldSpace = input.normal;
	float4 normalTangentSpace = (2 * normalMapTexture.Sample(samplerState, input.texcoord)) - 1.0f;

	float4 normals[2] = { normalWorldSpace, normalTangentSpace };

	output.normal = normals[index];

	// All color components are summed in the pixel shader.
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	output.color = (textureColor * factor + baseColor * (1.0f - factor));;

	return output;
}