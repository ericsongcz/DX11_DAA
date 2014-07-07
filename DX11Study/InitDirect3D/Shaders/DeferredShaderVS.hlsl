cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 worldViewProjectionMatrix;
};

cbuffer CommonBuffer : register(b1)
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
};


struct VertexInput
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	// 顶点坐标扩展成四个分量，并设置为1，以便矩阵运算。
	input.position.w = 1.0f;

	// 乘以3个矩阵，得到clip空间的坐标。
	// 保存worldPosition以便光照计算。
	output.worldPosition = mul(input.position, worldMatrix);
	output.position = mul(input.position, worldViewProjectionMatrix);

	// 因为normal使用的float4，如果不将w设为0，会导致光照计算错误。
	// 切记切记，血泪的教训啊！
	input.normal.w = 0.0f;
	input.tangent.w = 0.0f;

	float3 tangentW = mul(input.tangent, worldMatrix).xyz;

	float3 N = normalize(mul(input.normal, worldMatrix)).xyz;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	//float3 B = normalize(mul(input.binormal, worldMatrix)).xyz;
	float3 B = cross(T, N);

	output.normal = N;
	output.tangent = T;
	output.binormal = B;

	//float3x3 TBN = float3x3(T, B, N);

	// 将光照向量和观察向量转换到切线空间。
	//float3 lightDirTangentSpace = /*normalize*/(mul(TBN, lightDirWorldSpace));
	//float3 viewDirTangentSpace = /*normalize*/(mul(TBN, viewDirWorldSpace));

	//float3 lightDirs[2] = { lightDirWorldSpace, lightDirTangentSpace };
	//float3 viewDirs[2] = { viewDirWorldSpace, viewDirTangentSpace };

	//output.lightDir = lightDirs[index];
	//output.viewDir = viewDirs[index];

	//output.lightDir = mul(TBN, lightDirection.xyz);

	output.texcoord = input.texcoord;

	return output;
}