#include "LightHelper.hlsli"

cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjection;
};

cbuffer LightBuffer : register(b1)
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

cbuffer CommonBuffer : register(b2)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	PointLight pointLight;
}

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float3 lightDir : NORMAL1;
	float3 viewDir : NORMAL2;
	float2 texcoord : TEXCOORD0;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalMapTexture : register(t1);

SamplerState samplerState : register(s0)
{
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 main(PixelInput input) : SV_TARGET
{
	float3 lightDir = normalize(input.lightDir);
	float3 viewDir = normalize(input.viewDir);

	float4 normalWorldSpace;
	float4 normalTangentSpace;

	if (hasNormalMapTexture)
	{
		normalTangentSpace = 2 * (normalMapTexture.Sample(samplerState, input.texcoord) - 0.5f);
	}
	else
	{
		normalWorldSpace = input.normal;
	}

	//float3 lightDir = /*normalize*/(lightPosition - input.worldPosition).xyz;
	//float3 viewDir = /*normalize*/(cameraPosition - input.worldPosition).xyz;

	//float4x4 worldToTangentSpace;
	//// 如果放到PS里的话这里的normal要用normal map采样的normal才会有正确结果(?)。
	//input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

	//worldToTangentSpace[0] = input.tangent;
	//worldToTangentSpace[1] = float4(cross(input.tangent.xyz, normalWorldSpace.xyz), 1.0f);
	//worldToTangentSpace[2] = normalWorldSpace;
	//worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//float4x4 tangentSpaceToWorld = transpose(worldToTangentSpace);

	//normalTangentSpace = mul(normalTangentSpace, worldToTangentSpace);

	float3 normals[2] = { normalWorldSpace.xyz, normalTangentSpace.xyz };

	float3 normal = normals[index];

	float diffuse = saturate(dot(lightDir, normal));

	// Calculate Phong components per-pixel.
	float3 reflectionVector = normalize(reflect(-lightDir, normal));

	// Manually compute reflection vector.
	// r = I - 2(N·L)N.
	//float3 reflectionVector = normalize(-lightDir - 2 * (dot(normal, -lightDir) * normal));

	// Calculate specular component.
	// specular = pow(max(v·r, 0), p)
	float4 specular = specularColor * pow(saturate(dot(reflectionVector, viewDir)), 50.0f);

	// All color components are summed in the pixel shader.
	float4 color;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = diffuseTexture.Sample(samplerState, input.texcoord);

	textureColor = (textureColor * factor + baseColor * (1.0f - factor));

	color = (ambientColor * ambientIntensity + diffuseColor * diffuse * diffuseIntensity) * textureColor/* + specular * 0.5f*/;

	return color;
}