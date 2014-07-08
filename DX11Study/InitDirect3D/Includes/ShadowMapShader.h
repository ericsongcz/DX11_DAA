#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "D3DUtils.h"
#include "CommonStructures.h"

using namespace DirectX;

class ShadowMapShader
{
public:

	ShadowMapShader();
	~ShadowMapShader();

	bool render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderShader();
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);
	void shutdown();
private:
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mLightBuffer;
	ID3D11InputLayout* mInputLayout;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
};