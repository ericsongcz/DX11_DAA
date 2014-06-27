#include "LightHelper.hlsli"

cbuffer LightBuffer : register(b0)
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

cbuffer FogBuffer : register(b1)
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

Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);

SamplerState samplerState : register(s0)
{
	//MipFilter = POINT;
	//MinFilter = POINT;
	//MagFilter = POINT;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//AddressW = Clamp;
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

	worldPosition.w = 1.0f;
	normal.w = 0.0f;

	float3 viewDir = (cameraPosition - worldPosition).xyz;

	LightResult result = ComputeDirectionalLight(directionalLight, normal.xyz, viewDir);
	float4 directionalLightDiffuseColor = result.diffuseColor;

	result = ComputePointLight(pointLight, normal.xyz, worldPosition.xyz, viewDir);
	float4 pointLightDiffuseColor = result.diffuseColor;

	result = ComputeSpotLight(spotLight, normal.xyz, worldPosition.xyz, viewDir);
	float4 spotLightDiffuseColor = result.diffuseColor;

	// All color components are summed in the pixel shader.
	float4 diffuseColor = diffuseColor = (directionalLightDiffuseColor/* + pointLightDiffuseColor*//* + spotLightDiffuseColor*/)* diffuseIntensity;
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = colorTexture.Sample(samplerState, input.texcoord);

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