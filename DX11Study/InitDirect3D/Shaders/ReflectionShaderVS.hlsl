cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};

cbuffer ReflectionBuffer : register(b1)
{
	float4x4 reflectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	// ����������չ���ĸ�������������Ϊ1���Ա�������㡣
	input.position.w = 1.0f;

	// ����3�����󣬵õ�clip�ռ�����ꡣ
	// ����worldPosition�Ա���ռ��㡣
	output.position = mul(input.position, worldViewProjectionMatrix);
	output.texcoord = input.texcoord;

	// Create the reflection projection world matrix.
	// ����д������һ���ġ�
	// float4x4 reflectProjectWorld = mul(worldMatrix, reflectionMatrix);
	// reflectProjectWorld = mul(reflectProjectWorld, projectionMatrix);
	float4x4 reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// Calculate the input position against the reflectProjectWorld matrix.
	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	return output;
}