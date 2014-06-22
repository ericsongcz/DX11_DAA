#include "LightHelper.hlsli"

// Shader中使用的全局变量都定义在const buffer中，
// 这样Shader编译后，这些变量放在GPU的const buffer中。
cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjection;
};

cbuffer LightBuffer : register(b1)
{
	float4 lightPosition;
	float4 lightDirection;
	float4 ambientColor;
	float4 diffuseColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	float4 cameraPosition;
	float4 specularColor;
};

cbuffer CommonBuffer : register(b2)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	PointLight pointLight;
}

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texcoord : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
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
	// 切记切记，血泪的教训啊！
	input.normal.w = 0.0f;
	input.tangent.w = 0.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjection);

	// 使用查表法移除if语句。

	// 如果在这里归一化向量会导致不正确的渲染结果(?)。
	float3 lightDirWorldSpace = /*normalize*/(lightPosition - output.worldPosition).xyz;
	float3 viewDirWorldSpace = /*normalize*/(cameraPosition - output.worldPosition).xyz;

	float3 tangentW = mul(input.tangent, worldMatrix).xyz;

	float3 N = normalize(mul(input.normal, worldMatrix)).xyz;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	//float3 B = normalize(mul(input.binormal, worldMatrix)).xyz;
	float3 B = cross(T, N);

	//output.normal = N;
	//output.tangent = T;
	//output.binormal = B;

	float3x3 TBN = float3x3(T, B, N);

	//float3x3 worldToTangentSpace;
	//worldToTangentSpace[0] = normalize(mul(input.tangent.xyz, (float3x3)worldMatrix));
	//worldToTangentSpace[1] = normalize(mul(cross(input.tangent.xyz, input.normal.xyz), (float3x3)worldMatrix));
	//worldToTangentSpace[2] = normalize(mul(input.normal.xyz, (float3x3)worldMatrix));
	////worldToTangentSpace[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 lightDirTangentSpace = /*normalize*/(mul(TBN, lightDirWorldSpace)).xyz;
	float3 viewDirTangentSpace = /*normalize*/(mul(TBN, viewDirWorldSpace)).xyz;

	float3 lightDirs[2] = { lightDirWorldSpace, lightDirTangentSpace };
	float3 viewDirs[2] = { viewDirWorldSpace, viewDirTangentSpace };

	output.lightDir = lightDirs[index];
	output.viewDir = viewDirs[index];

	//output.normal = normalize(mul(input.normal, worldMatrix));
	//output.tangent = normalize(mul(input.tangent, worldMatrix));

	// 直接输出顶点的颜色（顶点之间的颜色，会在光栅化阶段采用插值的方式计算）。
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}