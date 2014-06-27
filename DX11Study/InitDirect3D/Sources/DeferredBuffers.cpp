#include "stdafx.h"
#include "DeferredBuffers.h"
#include "D3DUtils.h"

DeferredBuffers::DeferredBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		mRenderTargetTextureArray[i] = nullptr;
		mRenderTargetViewArray[i] = nullptr;
		mShaderResourceViewArray[i] = nullptr;
	}

	mDepthSencilBuffer = nullptr;
	mDepthStencilView = nullptr;
}

DeferredBuffers::~DeferredBuffers()
{
	shutdown();
}

bool DeferredBuffers::initialize(ID3D11Device* device, int textureWidth, int textureHeight, float depth, float nearZ)
{
	shutdown();

	mTextureWidth = textureWidth;
	mTextureHeight = textureHeight;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = mTextureWidth;
	textureDesc.Height = mTextureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target textures.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		HR(device->CreateTexture2D(&textureDesc, nullptr, &mRenderTargetTextureArray[i]));
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		HR(device->CreateRenderTargetView(mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i]));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		HR(device->CreateShaderResourceView(mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i]));
	}

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthBufferDesc.Width = mTextureWidth;
	depthBufferDesc.Height = mTextureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create depth buffer.
	HR(device->CreateTexture2D(&depthBufferDesc, nullptr, &mDepthSencilBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	HR(device->CreateDepthStencilView(mDepthSencilBuffer, &depthStencilViewDesc, &mDepthStencilView));

	// Setup the viewport for rendering.
	mViewport.Width = (float)mTextureWidth;
	mViewport.Height = (float)mTextureHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;

	return true;
}

void DeferredBuffers::shutdown()
{
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthSencilBuffer);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		SafeRelease(mShaderResourceViewArray[i]);
		SafeRelease(mRenderTargetViewArray[i]);
		SafeRelease(mRenderTargetTextureArray[i]);
	}
}

void DeferredBuffers::setRenderTargets(ID3D11DeviceContext* deviceConext)
{
	// Bind the render target view array and depth stencil buffer to the output render pipeline.
	deviceConext->OMSetRenderTargets(BUFFER_COUNT, mRenderTargetViewArray, mDepthStencilView);

	// Set the viewport.
	deviceConext->RSSetViewports(1, &mViewport);
}

void DeferredBuffers::clearRenderTargets(ID3D11DeviceContext* deviceContext, float clearColor[4])
{
	// Clear the render target buffers.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(mRenderTargetViewArray[i], clearColor);
	}

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
}

ID3D11ShaderResourceView* DeferredBuffers::getShaderResourceView(int index)
{
	return mShaderResourceViewArray[index];
}