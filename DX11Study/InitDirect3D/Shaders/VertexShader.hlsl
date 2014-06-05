// Shader��ʹ�õ�ȫ�ֱ�����������const buffer�У�
// ����Shader�������Щ��������GPU��const buffer�С�
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
	float4 position : SV_POSITION;	// SV����ϵͳ�Զ���ĸ�ʽ��
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
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
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.normal = mul(input.normal, worldMatrix);

	// ֱ������������ɫ������֮�����ɫ�����ڹ�դ���׶β��ò�ֵ�ķ�ʽ���㣩��
	output.color = input.color;
	output.texcoord = input.texcoord;

	//output.position.x *= scaleFactor;
	//output.position.y *= scaleFactor;
	//output.position.z *= scaleFactor;

	return output;
}