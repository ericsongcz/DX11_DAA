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

cbuffer CommonBuffer : register(b1)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	int showDepthComplexity;
	int showShadow;
	int commonBufferPad2;
	int commonBufferPad3;
}

cbuffer FogBuffer : register(b2)
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

Texture2D diffuseTexture : register(t0);
Texture2D normalMapTexture : register(t1);
Texture2D depthMapTexture : register(t2);

SamplerState samplerStateWrap : register(s0);
SamplerState samplerStateClamp : register(s1);

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 lightViewPosition : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 texcoord : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 outputColor = ambientColor * ambientIntensity;

	// Calculate the amount of light on this pixel.
	float3 viewDir = (cameraPosition - input.worldPosition).xyz;

	float2 projectTexCoord;

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range. If so then this pixel is in the light view.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		float depth = 1.0f;

		if (showShadow)
		{
			depth = depthMapTexture.Sample(samplerStateClamp, projectTexCoord).r;
		}

		// Calculate the depth of the light.
		float lightDepth = input.lightViewPosition.z / input.lightViewPosition.w;

		// Substract the bias from the lightDepth.
		float bias = 0.001f;
		lightDepth -= bias;

		// Compare the depth of the shadow map value and the depth of the light to 
		// determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then
		// shadow this pixel since an object(occluder) is casting a shadow on it.
		if (lightDepth < depth)
		{
			float3 normalWorldSpace = float3(0.0f, 0.0f, 0.0f);
			float3 normalTangentSpace = float3(0.0f, 0.0f, 0.0f);

			if (hasNormalMapTexture)
			{
				normalTangentSpace = 2 * (normalMapTexture.Sample(samplerStateWrap, input.texcoord).xyz - 0.5f);
			}
			else
			{
				normalWorldSpace = input.normal;
			}

			float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);

			// 这里如果左乘的话相当于乘以TBN的转置矩阵, 也就是从世界空间转换到切线空间。
			// 将切线空间的法线转换到世界空间。
			normalTangentSpace = mul(normalTangentSpace, TBN);
			normalTangentSpace = normalize(normalTangentSpace);

			float3 normals[2] = { normalWorldSpace, normalTangentSpace };

			float3 normal = normals[index];

			LightResult result = ComputeDirectionalLight(directionalLight, normal, viewDir);
			float4 directionalLightDiffuseColor = result.diffuseColor;

			float4 pointLightDiffuseColor = DirectIllumination(pointLight, input.worldPosition.xyz, normal, 0.001f);

			// All color components are summed in the pixel shader.
			float4 diffuseColor = (directionalLightDiffuseColor/* + pointLightDiffuseColor *//* + spotLightDiffuseColor*/) * diffuseIntensity;

			outputColor += diffuseColor;
		}
	}

	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = diffuseTexture.Sample(samplerStateWrap, input.texcoord);

	textureColor = (textureColor * factor + baseColor * (1.0f - factor));

	outputColor *= textureColor;

	if (showFog)
	{
		float fogEnd = fogStart + fogRange;
		float distanceToEye = length(viewDir);
		float fogLerp = 0;

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