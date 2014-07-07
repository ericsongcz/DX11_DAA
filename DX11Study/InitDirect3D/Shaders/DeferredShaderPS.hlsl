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
	//Filter = MIN_MAG_MIP_LINEAR;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//AddressW = Clamp;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
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

	float3 normalWorldSpace = float3(0.0f, 0.0f, 0.0f);
	float3 normalTangentSpace = float3(0.0f, 0.0f, 0.0f);

	if (hasNormalMapTexture)
	{
		normalTangentSpace = 2 * (normalMapTexture.Sample(samplerState, input.texcoord).xyz - 0.5f);
	}
	else
	{
		normalWorldSpace = input.normal;
	}

	float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);

	// 这里如果坐乘的话相当于乘以TBN的转置矩阵。
	// 将切线空间的法线转换到世界空间。
	normalTangentSpace = mul(normalTangentSpace, TBN);
	normalTangentSpace = normalize(normalTangentSpace);

	float3 normals[2] = { normalWorldSpace, normalTangentSpace };

	output.normal = float4(normals[index], 0.0f);

	// All color components are summed in the pixel shader.
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	output.color = (textureColor * factor + baseColor * (1.0f - factor));

	return output;
}