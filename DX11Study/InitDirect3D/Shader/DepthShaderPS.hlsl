struct PixelInput
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
	float depth = input.depthPosition.z / input.depthPosition.w;

	float4 color = float4(depth, depth, depth, 1.0f);

	return color;
}