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

struct PixelInput
{
	float4 position : SV_POSITION;	// SV代表系统自定义的格式。
	float4 worldPosition : POSITION;
	float4 color : COLOR;
	float4 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState samplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
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
	float3 normal = normalize(input.normal.xyz);
	float3 directionToLight = normalize(lightPosition - input.worldPosition).xyz;
	float diffuseIntensity = saturate(dot(directionToLight, normal));
	float4 diffuse = diffuseColor * diffuseIntensity;

	// Calculate Phong components per-pixel.
	float3 reflectionVector = normalize(reflect(-directionToLight, normal));

	// Manually compute reflection vector.
	// r = I - 2(N·L)N.
	//float3 reflectionVector = normalize(-directionToLight - 2 * (dot(input.normal.xyz, -directionToLight) * input.normal.xyz));

	float3 directionToCamera = normalize(cameraPosition - input.worldPosition).xyz;

	// Calculate specular component.
	// specular = pow(max(v·r, 0), p)
	float4 specular = specularColor * pow(saturate(dot(reflectionVector, directionToCamera)), 50);

	// All color components are summed in the pixel shader.
	float4 ambientLightColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

	float4 textureColor = shaderTexture.Sample(samplerState, input.texcoord);
	float4 color;

	//if (!float4Equal(textureColor, float4(0.0f, 0.0f, 0.0f, 0.0f)))
	//{
		color = (ambientLightColor + diffuse) * textureColor + specular * 0.5f;
	//}
	//else
	//{
		//color = ambientLightColor * 0.3f + diffuse * 0.8f + specular * 0.5f;
	//}

	return color;
}