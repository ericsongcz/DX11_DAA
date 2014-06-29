Texture2D diffuseTexture;
SamplerState samplerState;

Texture2D reflectionTexture;

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = reflectionTexture.Sample(samplerState, input.texcoord);

	//  输入的反射位置的齐次坐标需要转换为适当的纹理坐标。为了做到这一点首先除以w，这让我们得到[-1, 1]之间的tu和tv，为了映射到[0, 1]，简单的除以2然后加上0.5。
	// Calculate the projected reflection texture coordinates.
	float2 reflectionTexcoord;
	reflectionTexcoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0 + 0.5f;
	reflectionTexcoord.y = input.reflectionPosition.y / input.reflectionPosition.w / 2.0 + 0.5f;

	// Sample the texture pixel from the reflection texture using the projected texture coordinates.
	float4 reflectionColor = reflectionTexture.Sample(samplerState, reflectionTexcoord);

	// Do a linear interpolation between the two textures for a blend effect.
	float4 outputColor = lerp(textureColor, reflectionColor, 0.15);

	return outputColor;
}