#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct MatrixBuffer
{
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4 color1;
	XMFLOAT4 color2;
	XMFLOAT4 color3;
	XMFLOAT4 color4;
};

class Shader
{
public:
	Shader();
	~Shader();

	bool render(FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	bool setShaderParameters(FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderShader();
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName);

	void setShaderResource(ID3D11ShaderResourceView* shaderResourceView);

	void shutdown();
private:
	ID3D11Buffer* mMatrixBuffer;
	ID3D11InputLayout* mInputLayout;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11SamplerState* mSamplerState;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11ShaderResourceView* mShaderResourceView;
};