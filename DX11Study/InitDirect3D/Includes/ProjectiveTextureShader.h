#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Geometry.h"
#include "D3DUtils.h"
#include "ShaderUntils.h"

using namespace DirectX;

class ProjectiveTextureShader
{
public:

	ProjectiveTextureShader();
	~ProjectiveTextureShader();

	bool render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderShader();
	void setSamplerState(ESamplerType samplerType);
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);
	void setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews);
	void shutdown();
private:
	ID3D11Buffer* mMatrixBuffer;
	ID3D11InputLayout* mInputLayout;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11SamplerState* mSamplerStateLinear;
	ID3D11SamplerState* mSamplerStateAnisotropic;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11ShaderResourceView* mShaderResourceView;
};