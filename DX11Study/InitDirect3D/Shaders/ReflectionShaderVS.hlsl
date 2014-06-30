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

	// 顶点坐标扩展成四个分量，并设置为1，以便矩阵运算。
	input.position.w = 1.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.position = mul(input.position, worldViewProjectionMatrix);
	output.texcoord = input.texcoord;

	// Create the reflection projection world matrix.
	// 两种写法都是一样的。
	// float4x4 reflectProjectWorld = mul(worldMatrix, reflectionMatrix);
	// reflectProjectWorld = mul(reflectProjectWorld, projectionMatrix);
	float4x4 reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// Calculate the input position against the reflectProjectWorld matrix.
	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	return output;
}