cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	// 顶点坐标扩展成四个分量，并设置为1，以便矩阵运算。
	input.position.w = 1.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.position = mul(input.position, worldViewProjectionMatrix);

	output.depthPosition = output.position;

	return output;
}