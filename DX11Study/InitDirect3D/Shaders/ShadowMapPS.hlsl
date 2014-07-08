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

Texture2D depthMapTexture : register(t0);

SamplerState samplerStateWrap : register(s0);
SamplerState samplerStateClamp : register(s1);

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 lightViewPosition : TEXCOORD1;
	float3 normal : NORMAL;
};

float4 main(PixelInput input) : SV_TARGET
{
	float2 projectTexCoord;

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	float4 outputColor = ambientColor * ambientIntensity;

	// Determine if the projected coordinates are in the 0 to 1 range. If so then this pixel is in the light view.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		float depth = depthMapTexture.Sample(samplerStateClamp, projectTexCoord).r;

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
			// Calculate the amount of light on this pixel.
			float3 viewDir = (cameraPosition - input.worldPosition).xyz;

			float3 normal = input.normal;

			LightResult result = ComputeDirectionalLight(directionalLight, normal, viewDir);
			float4 directionalLightDiffuseColor = result.diffuseColor;

			float4 pointLightDiffuseColor = DirectIllumination(pointLight, input.worldPosition.xyz, normal, 0.001f);

			// All color components are summed in the pixel shader.
			float4 diffuseColor = (/*directionalLightDiffuseColor + */pointLightDiffuseColor/* + spotLightDiffuseColor*/)* diffuseIntensity;

			outputColor += diffuseColor;
		}
	}

	return outputColor;
}