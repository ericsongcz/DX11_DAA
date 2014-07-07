#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Geometry.h"
#include "ShaderUntils.h"

using namespace DirectX;

class DeferredShader
{
public:
	DeferredShader();
	~DeferredShader();

	bool render(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderShader();
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);
	void shutdown();
private:
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mCommonBuffer;
	ID3D11InputLayout* mInputLayout;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11ShaderResourceView* mShaderResourceView;
};
