struct DirectionalLight
{
	float4 diffuseColor;
	float4 direction;
};

struct PointLight
{
	float4 diffuseColor;
	float4 position;
	float radius;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};

struct SpotLight
{
	float4 diffuseColor;
	float3 position;
	float radius;
	float3 direction;
	float spot;
};

struct LightResult
{
	float4 diffuseColor;
	float4 specularColor;
};

float diffuseToonShading(float diffuseFactor)
{
	float result = 0.0f;

	if (diffuseFactor <= 0.0f)
	{
		result = 0.4;
	}
	else if (diffuseFactor > 0 && diffuseFactor <= 0.5f)
	{
		result = 0.6f;
	}
	else if (diffuseFactor > 0.5f && diffuseFactor <= 1.0f)
	{
		result = 1.0f;
	}

	return result;
}

float specularToonShading(float specularFactor)
{
	float result = 0.0f;

	if (specularFactor >= 0.0f && specularFactor <= 0.1f)
	{
		result = 0.0f;
	}
	else if (specularFactor > 0.1f && specularFactor <= 0.8f)
	{
		result = 0.5f;
	}
	else if (specularFactor > 0.8f && specularFactor <= 1.0f)
	{
		result = 1.0f;
	}

	return result;
}

float SpecularFactor(float3 reflectVector, float3 toEye, float power)
{
	// Calculate specular component.
	// specular = pow(max(v，r, 0), p)
	return pow(saturate(dot(reflectVector, toEye)), power);
}

// Reference.
// Light attenuation
// http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
float4 DirectIllumination(PointLight light, float3 position, float3 normal, float cutoff)
{
	// calculate normalized light vector and distance to sphere light surface
	float radius = light.radius;
	float3 lightDir = light.position.xyz - position;
	float distance = length(lightDir);
	float d = max(distance - radius, 0);
	lightDir /= distance;

	// calculate basic attenuation
	float denom = d / radius + 1;
	float attenuation = 1 / (denom * denom);

	// scale and bias attenuation such that:
	//   attenuation == 0 at extent of max influence
	//   attenuation == 1 when d == 0
	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0);

	float diffuseFactor = max(dot(lightDir, normal), 0);
	return light.diffuseColor * diffuseFactor * attenuation;
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

	if (distance > light.radius)
	{
		return result;
	}

	lightDir = normalize(lightDir);

	float diffuseFactor = saturate(dot(lightDir, normal));
	// Toon Shading.
	//float diffuseFactor = diffuseToonShading(dot(lightDir, normal));
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
	//float attenuation = 1.0f - dot(lightDir / distance, lightDir / distance);
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
