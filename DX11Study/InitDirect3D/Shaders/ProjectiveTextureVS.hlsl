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
	float4 position : SV_POSITION;	// SV����ϵͳ�Զ���ĸ�ʽ��
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
	float4 viewPosition : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	// ����������չ���ĸ�������������Ϊ1���Ա�������㡣
	input.position.w = 1.0f;

	// ��Ϊnormalʹ�õ�float4���������w��Ϊ0���ᵼ�¹��ռ������
	// �м��мǣ�Ѫ��Ľ�ѵ����
	input.normal.w = 0.0f;

	// ����3�����󣬵õ�clip�ռ�����ꡣ
	// ����worldPosition�Ա���ռ��㡣
	output.position = mul(input.position, worldViewProjectionMatrix);

	// Store the position of the vertices as viewd by the projection view point
	// in a separate variable.
	output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.position, viewMatrix2);
	output.viewPosition = mul(output.position, projectionMatrix2);

	return output;
}