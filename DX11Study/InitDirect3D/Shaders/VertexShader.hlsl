// Shader中使用的全局变量都定义在const buffer中，
// 这样Shader编译后，这些变量放在GPU的const buffer中。
cbuffer MatrixBuffer
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 lightPosition;
	float4 diffuseColor;
	float4 cameraPosition;
	float4 specularColor;
};

cbuffer Test
{
	float scaleFactor;
	float scaleFactor1;
	float scaleFactor2;
	float scaleFactor3;
	bool hasTexture;
	bool dummy1;
	bool dummy2;
	bool dummy3;
	float dummy4;
	float dummy5;
	float dummy6;
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
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	// 顶点坐标扩展成四个分量，并设置为1，以便矩阵运算。
	input.position.w = 1.0f;

	// 因为normal使用的float4，如果不将w设为0，会导致光照计算错误。
	input.normal.w = 0.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = mul(input.normal, worldMatrix);

	// 直接输出顶点的颜色（顶点之间的颜色，会在光栅化阶段采用插值的方式计算）。
	output.color = input.color;
	output.texcoord = input.texcoord;

	//output.position.x *= scaleFactor;
	//output.position.y *= scaleFactor;
	//output.position.z *= scaleFactor;

	return output;
}