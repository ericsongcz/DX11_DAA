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
	// Calculate specular component.
	// specular = pow(max(v，r, 0), p)
	return pow(saturate(dot(reflectVector, toEye)), power);
}

LightResult ComputeDirectionalLight(DirectionalLight light, float3 normal, float3 toEye)
{
	LightResult result = (LightResult)0;
	float diffuseFactor = saturate(dot(normal, -light.direction.xyz));
	float4 diffuseColor = light.diffuseColor * diffuseFactor;

	//[flatten]
	//if (diffuseFactor > 0.0f)
	//{}

	// Manually compute reflection vector.
	//  r = I - 2(N，L)N.
	//float3 reflectionVector = normalize(-lightDir - 2 * (dot(normal, -lightDir) * normal));

	float3 reflectVector = reflect(-light.direction.xyz, normal);
	float specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	result.diffuseColor = diffuseColor;
	result.specularColor = specualarColor;

	return result;
}

LightResult ComputePointLight(PointLight light, float3 normal, float3 position, float3 toEye)
{
	LightResult result = (LightResult)0;

	float3 lightDir = light.position.xyz - position;
	float distance = length(lightDir);

	if (distance > light.range)
	{
		return result;
	}

	lightDir = normalize(lightDir);

	float diffuseFactor = saturate(dot(lightDir, normal));
	float4 diffuseColor = light.diffuseColor * diffuseFactor;

	//[flatten]
	//if (diffuseFactor > 0.0f)
	//diffuseColor = light.diffuseColor * diffuseFactor;
	//{}

	// Manually compute reflection vector.
	//  r = I - 2(N，L)N.
	//float3 reflectionVector = normalize(-lightDir - 2 * (dot(normal, -lightDir) * normal));

	float3 reflectVector = reflect(-lightDir, normal);
	float specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	// Attenuate.
	float3 attenuations = float3(light.attenuation0, light.attenuation1, light.attenuation2);
	float attenuation = 1.0f / dot(attenuations, float3(1.0f, distance, distance * distance));
	result.diffuseColor = diffuseColor * attenuation;
	result.specularColor = specualarColor * attenuation;

	return result;
}

LightResult ComputeSpotLight(SpotLight light, float3 normal, float3 position, float3 toEye)
{
	LightResult result = (LightResult)0;

	float3 lightDir = normalize(light.position - position);
	float spotFactor = pow(saturate(dot(-lightDir, light.direction)), light.spot);
	float diffuseFactor = saturate(dot(lightDir, normal));
	float4 diffuseColor = light.diffuseColor * diffuseFactor * spotFactor;

	//[flatten]
	//if (diffuseFactor > 0.0f)
	//{}

	// Manually compute reflection vector.
	//  r = I - 2(N，L)N.
	//float3 reflectionVector = normalize(-lightDir - 2 * (dot(normal, -lightDir) * normal));

	float3 reflectVector = reflect(-lightDir, normal);
	float specualarFactor = SpecularFactor(reflectVector, toEye, 8.0f);
	float4 specualarColor = light.diffuseColor * specualarFactor;

	result.diffuseColor = diffuseColor;
	result.specularColor = specualarColor;

	return result;
}