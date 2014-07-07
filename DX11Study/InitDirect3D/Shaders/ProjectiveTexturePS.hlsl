Texture2D projectiveTexture : register(t0);

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
	float4 viewPosition : TEXCOORD1;
};

SamplerState samplerState : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
	float2 projectTexCoord;
	projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

	float4 outputColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

	// Determine if the projected coordinates are in the 0 to 1 range.
	// If it is the pixel is inside the projected view port.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		outputColor = projectiveTexture.Sample(samplerState, projectTexCoord);
	}

	return outputColor;
}