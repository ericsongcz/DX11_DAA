#include "LightHelper.hlsli"

cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
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
}

cbuffer FogBuffer : register(b3)
{
	float4 fogColor;
	float fogStart;
	float fogRange;
	float fogDensity;
	int fogType;
	int showFog;
	int fogBufferPad1;
	int fogBufferPad2;
	int fogBufferPad3;
}

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 lightDir : NORMAL1;
	float3 viewDir : NORMAL2;
	float2 texcoord : TEXCOORD0;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalMapTexture : register(t1);

SamplerState samplerState : register(s0)
{
	//MipFilter = LINEAR;
	//MinFilter = LINEAR;
	//MagFilter = LINEAR;
	//AddressU = Wrap;
	//AddressV = Wrap;
	//AddressW = Wrap;
};

float4 main(PixelInput input) : SV_TARGET
{
	//float3 lightDir = normalize(input.lightDir);
	//float3 viewDir = normalize(input.viewDir);

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

	// 这里如果左乘的话相当于乘以TBN的转置矩阵。
	// 将切线空间的法线转换到世界空间。
	normalTangentSpace = mul(normalTangentSpace, TBN);
	normalTangentSpace = normalize(normalTangentSpace);

	float3 normals[2] = { normalWorldSpace, normalTangentSpace };

	float3 normal = normals[index];

	float3 viewDir = (cameraPosition - input.worldPosition).xyz;

	LightResult result = ComputeDirectionalLight(directionalLight, normal, viewDir);
	float4 directionalLightDiffuseColor = result.diffuseColor;

	//result = ComputePointLight(pointLight, normal, input.worldPosition.xyz, viewDir);
	//float4 pointLightDiffuseColor = result.diffuseColor;
	float4 pointLightDiffuseColor = DirectIllumination(pointLight, input.worldPosition.xyz, normal, 0.001f);

	result = ComputeSpotLight(spotLight, normal, input.worldPosition.xyz, viewDir);
	float4 spotLightDiffuseColor = result.diffuseColor;

	// All color components are summed in the pixel shader.
	float4 diffuseColor = (directionalLightDiffuseColor/* + pointLightDiffuseColor*//* + spotLightDiffuseColor*/) * diffuseIntensity;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = diffuseTexture.Sample(samplerState, input.texcoord);

	textureColor = (textureColor * factor + baseColor * (1.0f - factor));

	float4 outputColor = (ambientColor * ambientIntensity + diffuseColor) * textureColor/* + specular * 0.5f*/;

	if (showFog)
	{
		float fogEnd = fogStart + fogRange;
		float distanceToEye = length(viewDir);
		float fogLerp;
		
		if (fogType == 0)
		{
			fogLerp = saturate((distanceToEye - fogStart) / fogRange);
			fogLerp = 1 - fogLerp * fogDensity;
		}

		if (fogType == 1)
		{
			fogLerp = 1.0f / exp(distanceToEye * fogDensity);
		}

		if (fogType == 2)
		{
			fogLerp = 1.0f / exp(pow(distanceToEye * fogDensity, 2));
		}

		outputColor = lerp(fogColor, outputColor, fogLerp);
	}

	return outputColor;
}