#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Geometry.h"
#include "ShaderUntils.h"

using namespace DirectX;

class DeferredShader
{
private:
	struct MatrixBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projectionMatrix;
		XMFLOAT4X4 worldViewProjection;
	};

	struct CommonBuffer
	{
		int hasDiffuseTexture;
		int hasNormalMapTexture;
		float factor;
		int index;
	};


public:
	DeferredShader();
	~DeferredShader();

	bool render(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews);
	void renderShader();
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);
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