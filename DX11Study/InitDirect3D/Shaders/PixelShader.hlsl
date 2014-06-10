cbuffer MatrixBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 lightPosition;
	float4 diffuseColor;
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

Texture2D diffuseTexture : register(t0);
Texture2D normalMapTexture : register(t1);

SamplerState samplerState : register(s0)
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
	float3 lightDir = input.lightDir;
	float3 viewDir = input.viewDir;

	float3 normalWorldSpace = input.normal.xyz;
	float3 normalTangentSpace = (2 * normalMapTexture.Sample(samplerState, input.texcoord)).xyz - 1.0f;

	float3 normals[2] = { normalWorldSpace, normalTangentSpace };

	float3 normal = normals[index];

	if (hasNormalMapTexture)
	{
		normal = (2 * normalMapTexture.Sample(samplerState, input.texcoord)).xyz - 1.0f;
	}
	else
	{
		normal = input.normal.xyz;
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
	float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 textureColor = diffuseTexture.Sample(samplerState, input.texcoord);

	textureColor = (textureColor * factor + baseColor * (1.0f - factor));

	color = (ambientLightColor + diffuse) * textureColor/* + specular * 0.5f*/;

	return color;
}