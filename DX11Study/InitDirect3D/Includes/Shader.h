#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Geometry.h"
#include "D3DUtils.h"
#include "ShaderUntils.h"

using namespace DirectX;

struct PointLight
{
	XMFLOAT4 lightPosition;
	float range;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};

struct CommonBuffer
{
	int hasDiffuseTexture;
	int hasNormalMapTexture;
	float factor;
	int index;
	PointLight pointLight;
};

class Shader
{
public:
	struct MatrixBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
		XMFLOAT4 lightPosition;
		XMFLOAT4 lightDirection;
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		float ambientIntensity;
		float diffuseIntensity;
		float pad1;
		float pad2;
		XMFLOAT4 cameraPositon;
		XMFLOAT4 specularColor;
		XMFLOAT4X4 worldViewProjection;
	};

	Shader();
	~Shader();

	bool render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderShader();
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);
	void setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews);
	void shutdown();
private:
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mCommonBuffer;
	ID3D11InputLayout* mInputLayout;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11SamplerState* mSamplerState;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11ShaderResourceView* mShaderResourceView;
};