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
	float4 normal = normalTexture.Sample(samplerState, input.texcoord);
	float4 tangent = tangentTexture.Sample(samplerState, input.texcoord);

	// 使用查表法移除if语句。
	// 如果在这里归一化向量会导致不正确的渲染结果(?)。
	float3 lightDirWorldSpace = /*normalize*/(lightPosition - position).xyz;
	float3 viewDirWorldSpace = /*normalize*/(cameraPosition - position).xyz;

	float4x4 worldToTangentSpace;
	worldToTangentSpace[0] = mul(tangent, worldMatrix);
	worldToTangentSpace[1] = mul(float4(cross(tangent.xyz, normal.xyz), 1.0f), worldMatrix);
	worldToTangentSpace[2] = mul(normal, worldMatrix);
	worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 lightDirTangentSpace = /*normalize*/(mul(worldToTangentSpace, float4(lightDirWorldSpace, 1.0f))).xyz;
	float3 viewDirTangentSpace = /*normalize*/(mul(worldToTangentSpace, float4(viewDirWorldSpace, 1.0f))).xyz;

	//float3 lightDirs[2] = { lightDirWorldSpace, lightDirTangentSpace };
	//float3 viewDirs[2] = { viewDirWorldSpace, viewDirTangentSpace };

	//float3 lightDir = normalize(lightDirs[index]);
	//float3 viewDir = normalize(viewDirs[index]);

	// Point light.
	//float3 lightDir = normalize(lightPosition - position).xyz;

	// Directional light.
	float3 lightDir = normalize(lightDirTangentSpace);

	float diffuse = saturate(dot(normal.xyz, lightDir));

	// All color components are summed in the pixel shader.
	float4 outputColor;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

	outputColor = (ambientColor * ambientIntensity + diffuseColor * diffuse * diffuseIntensity) * textureColor/* + specular * 0.5f*/;

	return outputColor;
}