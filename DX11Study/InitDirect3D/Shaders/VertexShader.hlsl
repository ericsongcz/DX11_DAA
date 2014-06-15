#include "LightHelper.hlsli"

// Shader中使用的全局变量都定义在const buffer中，
// 这样Shader编译后，这些变量放在GPU的const buffer中。
cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 lightPosition;
	float4 ambientColor;
	float4 diffuseColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	float4 cameraPosition;
	float4 specularColor;
	float4x4 worldViewProjection;
};

cbuffer Test : register(b1)
{
	bool hasDiffuseTexture;
	bool hasNormalMapTexture;
	float factor;
	int index;
	PointLight pointLight;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texcoord : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float3 lightDir : NORMAL1;
	float3 viewDir : NORMAL2;
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
	output.position = mul(input.position, worldViewProjection);

	// 使用查表法移除if语句。

	// 如果在这里归一化向量会导致不正确的渲染结果(?)。
	float3 lightDirWorldSpace = /*normalize*/(pointLight.lightPosition - output.worldPosition).xyz;
	float3 viewDirWorldSpace = /*normalize*/(cameraPosition - output.worldPosition).xyz;

	float4x4 worldToTangentSpace;
	worldToTangentSpace[0] = mul(input.tangent, worldMatrix);
	worldToTangentSpace[1] = mul(float4(cross(input.tangent.xyz, input.normal.xyz), 1.0f), worldMatrix);
	worldToTangentSpace[2] = mul(input.normal, worldMatrix);
	worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 lightDirTangentSpace = /*normalize*/(mul(worldToTangentSpace, float4(lightDirWorldSpace, 1.0f))).xyz;
	float3 viewDirTangentSpace = /*normalize*/(mul(worldToTangentSpace, float4(viewDirWorldSpace, 1.0f))).xyz;

	float3 lightDirs[2] = { lightDirWorldSpace, lightDirTangentSpace };
	float3 viewDirs[2] = { viewDirWorldSpace, viewDirTangentSpace };

	output.lightDir = lightDirs[index];
	output.viewDir = viewDirs[index];

	output.normal = normalize(mul(input.normal, worldMatrix));

	// 直接输出顶点的颜色（顶点之间的颜色，会在光栅化阶段采用插值的方式计算）。
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}