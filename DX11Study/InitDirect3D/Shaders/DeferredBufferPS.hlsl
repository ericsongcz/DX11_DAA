cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};


cbuffer CommonBuffer : register(b1)
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
	float4 tangent : TANGENT;
};

struct PixelOutput
{
	float4 position : SV_TARGET0;
	float4 color : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 tangent : SV_TARGET3;
};

PixelOutput main(PixelInput input)
{
	PixelOutput output;

	output.position = input.worldPosition;

	float4 normalWorldSpace = input.normal;
	float4 normalTangentSpace = 2 * (normalMapTexture.Sample(samplerState, input.texcoord) - 0.5f);

	normalTangentSpace = normalize(normalTangentSpace);

	float4x4 worldToTangentSpace;
	//worldToTangentSpace[0] = mul(input.tangent, worldMatrix);
	//worldToTangentSpace[1] = mul(float4(cross(input.tangent.xyz, normalWorldSpace.xyz), 1.0f), worldMatrix);
	//worldToTangentSpace[2] = mul(normalWorldSpace, worldMatrix);
	//worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

	worldToTangentSpace[0] = input.tangent;
	worldToTangentSpace[1] = float4(cross(input.tangent.xyz, normalWorldSpace.xyz), 1.0f);
	worldToTangentSpace[2] = normalWorldSpace;
	worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float4x4 tangentSpaceToWorld = transpose(worldToTangentSpace);

	normalTangentSpace = mul(normalTangentSpace, worldToTangentSpace);

	float4 normals[2] = { normalWorldSpace, normalTangentSpace };

	output.normal = normals[index];

	// All color components are summed in the pixel shader.
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	output.color = (textureColor * factor + baseColor * (1.0f - factor));

	output.tangent = input.tangent;

	return output;
}