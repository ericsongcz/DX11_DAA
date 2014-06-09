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
	bool hasDiffuseTexture;
	bool hasNormalMapTexture;
	bool dummy2;
	bool dummy3;
	float dummy4;
	float dummy5;
	float dummy6;
};

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float3 lightDir : NORMAL1;
	float3 viewDir : NORMAL2;
	float2 texcoord : TEXCOORD0;
};

Texture2D diffuseTexture;
Texture2D normalMapTexture;

SamplerState samplerState
{
	MipFilter = ANISOTROPIC;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

bool float4Equal(float4 lhs, float4 rhs)
{
	if ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w))
	{
		return true;
	}
	else
	{
		return false;
	}
}

float4 main(PixelInput input) : SV_TARGET
{
	// Calculate per-pixel diffuse.
	float3 normal;
	float3 lightDir = normalize(input.lightDir);
	float3 viewDir = normalize(input.viewDir);

	if (hasNormalMapTexture)
	{
		normal = (2 * normalMapTexture.Sample(samplerState, input.texcoord)).xyz - 1.0f;
	}
	else
	{
		normal = normalize(input.normal.xyz);
	}

	float diffuseIntensity = saturate(dot(lightDir, normal));
	float4 diffuse = diffuseColor * diffuseIntensity;

	// Calculate Phong components per-pixel.
	float3 reflectionVector = normalize(reflect(-lightDir, normal));

	// Manually compute reflection vector.
	// r = I - 2(N·L)N.
	//float3 reflectionVector = normalize(-lightDir - 2 * (dot(normal, -lightDir) * normal));

	// Calculate specular component.
	// specular = pow(max(v·r, 0), p)
	float4 specular = specularColor * pow(saturate(dot(reflectionVector, viewDir)), 50.0f);

	// All color components are summed in the pixel shader.
	float4 ambientLightColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float4 color;

	if (hasDiffuseTexture)
	{
		float4 textureColor = diffuseTexture.Sample(samplerState, input.texcoord);
		color = (ambientLightColor + diffuse) * textureColor/* + specular * 0.5f*/;
	}
	else
	{
		color = ambientLightColor * 0.3f + diffuse * 0.8f + specular * 0.5f;
	}

	return color;
}