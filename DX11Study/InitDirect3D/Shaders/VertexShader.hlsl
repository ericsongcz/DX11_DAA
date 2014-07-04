#include "LightHelper.hlsli"

// Shader��ʹ�õ�ȫ�ֱ�����������const buffer�У�
// ����Shader�������Щ��������GPU��const buffer�С�
cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
	float4x4 textureTransformMatrix;
};

cbuffer LightBuffer : register(b1)
{
	float4 ambientColor;
	float ambientIntensity;
	float diffuseIntensity;
	float pad1;
	float pad2;
	float4 cameraPosition;
	float4 specularColor;
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

cbuffer CommonBuffer : register(b2)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	int showDepthComplexity;
	int commonBufferPad1;
	int commonBufferPad2;
	int commonBufferPad3;
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
	float4 position : SV_POSITION;	// SV����ϵͳ�Զ���ĸ�ʽ��
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 lightDir : NORMAL1;
	float3 viewDir : NORMAL2;
	float2 texcoord : TEXCOORD0;
	float4 depthPosition : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	// ����������չ���ĸ�������������Ϊ1���Ա�������㡣
	input.position.w = 1.0f;

	// ��Ϊnormalʹ�õ�float4���������w��Ϊ0���ᵼ�¹��ռ������
	// �м��мǣ�Ѫ��Ľ�ѵ����
	input.normal.w = 0.0f;
	input.tangent.w = 0.0f;

	// ����3�����󣬵õ�clip�ռ�����ꡣ
	// ����worldPosition�Ա���ռ��㡣
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjectionMatrix);

	// ʹ�ò���Ƴ�if��䡣

	// ����������һ�������ᵼ�²���ȷ����Ⱦ���(?)��
	float3 lightDirWorldSpace = /*normalize*/(pointLight.position - output.worldPosition).xyz;
	float3 viewDirWorldSpace = /*normalize*/(cameraPosition - output.worldPosition).xyz;

	float3 tangentW = mul(input.tangent, worldMatrix).xyz;

	float3 N = normalize(mul(input.normal, worldMatrix)).xyz;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	//float3 B = normalize(mul(input.binormal, worldMatrix)).xyz;
	float3 B = cross(T, N);

	output.normal = N;
	output.tangent = T;
	output.binormal = B;

	// �����������͹۲������任�����߿ռ䡣
	float3x3 TBN = float3x3(T, B, N);

	// ����൱�ڳ���TBN��ת�þ���Ҳ����TBN�������
	// TBN�Ǵ����߿ռ䵽����ռ��ת����������任�Ļ�����Ҫ�����
	float3 lightDirTangentSpace = /*normalize*/(mul(TBN, lightDirWorldSpace));
	float3 viewDirTangentSpace = /*normalize*/(mul(TBN, viewDirWorldSpace));

	float3 lightDirs[2] = { lightDirWorldSpace, lightDirTangentSpace };
	float3 viewDirs[2] = { viewDirWorldSpace, viewDirTangentSpace };

	output.lightDir = lightDirs[index];
	output.viewDir = viewDirs[index];

	//output.lightDir = mul(TBN, lightDirection.xyz);

	// ֱ������������ɫ������֮�����ɫ�����ڹ�դ���׶β��ò�ֵ�ķ�ʽ���㣩��
	output.color = input.color;
	output.texcoord = mul(float4(input.texcoord, 0.0f, 1.0f), textureTransformMatrix).xy;
	output.texcoord = input.texcoord;
	output.depthPosition = output.position;

	return output;
}