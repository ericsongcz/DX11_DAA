cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
	float4x4 textureTransformMatrix;
	float4x4 viewMatrix2;
	float4x4 projectionMatrix2;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
	float4 viewPosition : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	// 顶点坐标扩展成四个分量，并设置为1，以便矩阵运算。
	input.position.w = 1.0f;

	// 因为normal使用的float4，如果不将w设为0，会导致光照计算错误。
	// 切记切记，血泪的教训啊！
	input.normal.w = 0.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.position = mul(input.position, worldViewProjectionMatrix);

	// Store the position of the vertices as viewd by the projection view point
	// in a separate variable.
	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.position, viewMatrix2);
	output.viewPosition = mul(output.position, projectionMatrix2);

	return output;
}