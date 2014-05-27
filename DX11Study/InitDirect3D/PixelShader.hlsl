cbuffer MatrixBuffer
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 color1;
	float4 color2;
	float4 color3;
	float4 color4;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 color : COLOR;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState samplerState;

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.texcoord);
	return textureColor;
}