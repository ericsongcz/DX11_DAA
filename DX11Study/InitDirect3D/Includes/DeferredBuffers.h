#pragma once
#include <d3d11.h>

const int BUFFER_COUNT = 3;

class DeferredBuffers
{
public:
	DeferredBuffers();
	~DeferredBuffers();

	bool initialize(ID3D11Device* device, int textureWidth, int textureHeight, float depth, float nearZ);
	void setRenderTargets(ID3D11DeviceContext* deviceConext);
	void clearRenderTargets(ID3D11DeviceContext* deviceContext, float clearColor[4]);
	ID3D11ShaderResourceView* getShaderResourceView(int index);
	void shutdown();
private:
	int mTextureWidth;
	int mTextureHeight;
	ID3D11Texture2D* mRenderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* mRenderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* mShaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* mDepthSencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mViewport;
};