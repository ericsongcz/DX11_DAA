struct PointLight
{
	float4 diffuseColor;
	float4 position;
	float range;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};

struct SpotLight
{
	float4 diffuseColor;
	float3 position;
	float range;
	float3 direction;
	float spot;
};