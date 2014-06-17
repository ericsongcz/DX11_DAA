Texture2D colorTexture : register(t0);

SamplerState samplerState : register(s0)
{
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 normal : NORMAL;
};

struct PixelOutput
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
};

PixelOutput main(PixelInput input)
{
	PixelOutput output;

	// Sample the color from the texture and store it for output to the render target.
	output.color = colorTexture.Sample(samplerState, input.texcoord);

	// Store the normal for output to the render target.
	output.normal = input.normal;

	return output;
}