#include "LightHelper.hlsli"

// Shader��ʹ�õ�ȫ�ֱ�����������const buffer�У�
// ����Shader�������Щ��������GPU��const buffer�С�
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
	float4 position : SV_POSITION;	// SV����ϵͳ�Զ���ĸ�ʽ��
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

	// ����������չ���ĸ�������������Ϊ1���Ա�������㡣
	input.position.w = 1.0f;

	// ��Ϊnormalʹ�õ�float4���������w��Ϊ0���ᵼ�¹��ռ������
	input.normal.w = 0.0f;

	// ����3�����󣬵õ�clip�ռ�����ꡣ
	// ����worldPosition�Ա���ռ��㡣
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjection);

	// ʹ�ò���Ƴ�if��䡣

	// ����������һ�������ᵼ�²���ȷ����Ⱦ���(?)��
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

	// ֱ������������ɫ������֮�����ɫ�����ڹ�դ���׶β��ò�ֵ�ķ�ʽ���㣩��
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}