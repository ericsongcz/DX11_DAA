struct DirectionalLight
{
	float4 diffuseColor;
	float4 direction;
};

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

struct LightResult
{
	float4 diffuseColor;
	float4 specularColor;
};

float SpecularFactor(float3 reflectVector, float3 toEye, float power)
{
	return pow(saturate(dot(reflectVector, toEye)), power);
}

LightResult ComputeDirectionalLight(DirectionalLight light, float3 normal, float3 toEye)
{
	LightResult result = (LightResult)0;
	float4 diffuseFactor = saturate(dot(normal, -light.direction.xyz));
	float4 diffuseColor = light.diffuseColor * diffuseFactor;

	float3 reflectVector = reflect(-light.direction.xyz, normal);
	float4 specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	result.diffuseColor = diffuseColor;
	result.specularColor = specualarColor;

	return result;
}

LightResult ComputePointLight(PointLight light, float3 normal, float3 position, float3 toEye)
{
	LightResult result = (LightResult)0;

	float3 lightDir = normalize(light.position.xyz - position);
	float4 diffuseFactor = saturate(dot(lightDir, normal));
	float4 diffuseColor = light.diffuseColor * diffuseFactor;

	float3 reflectVector = reflect(-lightDir, normal);
	float4 specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	result.diffuseColor = diffuseColor;
	result.specularColor = specualarColor;

	return result;
}

LightResult ComputeSpotLight(SpotLight light, float3 normal, float3 position, float3 toEye)
{
	LightResult result = (LightResult)0;

	float3 lightDir = normalize(light.position - position);
	float spotFactor = pow(saturate(dot(-lightDir, light.direction)), light.spot);
	float4 diffuseFactor = saturate(dot(lightDir, normal));
	float4 diffuseColor = light.diffuseColor * diffuseFactor * spotFactor;

	float3 reflectVector = reflect(-lightDir, normal);
	float4 specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	result.diffuseColor = diffuseColor;
	result.specularColor = specualarColor;

	return result;
}