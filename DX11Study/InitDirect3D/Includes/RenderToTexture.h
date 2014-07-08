#pragma once
#include <d3d11.h>

class RenderToTexture
{
public:
	RenderToTexture();
	~RenderToTexture();
	bool initialize(ID3D11Device* device, int width, int height);
	void setRenderTarget(ID3D11DeviceContext* deviceContext);
	void clearRenderTarget(ID3D11DeviceContext* deviceContext, float clearColor[4]);
	ID3D11ShaderResourceView* getShaderResourceView() const;
	void shutdown();
private:
	ID3D11Texture2D* mRenderTexture;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11ShaderResourceView* mShaderResourceView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mViewport;
};