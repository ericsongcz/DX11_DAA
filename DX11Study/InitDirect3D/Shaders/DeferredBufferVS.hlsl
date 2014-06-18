cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.position.w = 1.0f;
	
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.texcoord = input.texcoord;

	return output;
}