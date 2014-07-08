#include "stdafx.h"
#include "RenderToTexture.h"
#include "D3DUtils.h"

RenderToTexture::RenderToTexture()
	: mRenderTexture(nullptr),
	  mRenderTargetView(nullptr),
	  mShaderResourceView(nullptr)
{
}

RenderToTexture::~RenderToTexture()
{
	shutdown();
}

bool RenderToTexture::initialize(ID3D11Device* device, int width, int height)
{
	shutdown();

	D3D11_TEXTURE2D_DESC renderTextureDesc;
	ZeroMemory(&renderTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	renderTextureDesc.Width = width;
	renderTextureDesc.Height = height;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.SampleDesc.Quality = 0;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;

	HR(device->CreateTexture2D(&renderTextureDesc, nullptr, &mRenderTexture));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture1D.MipSlice = 0;

	HR(device->CreateRenderTargetView(mRenderTexture, &renderTargetViewDesc, &mRenderTargetView));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = renderTargetViewDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(mRenderTexture, &shaderResourceViewDesc, &mShaderResourceView));

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	HR(device->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	HR(device->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView));

	// Setup the viewport for rendering.
	mViewport.Width = (float)width;
	mViewport.Height = (float)height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;

	return true;
}

void RenderToTexture::setRenderTarget(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	deviceContext->RSSetViewports(1, &mViewport);
}

void RenderToTexture::clearRenderTarget(ID3D11DeviceContext* deviceContext, float clearColor[4])
{
	deviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderToTexture::getShaderResourceView() const
{
	return mShaderResourceView;
}

void RenderToTexture::shutdown()
{
	SafeRelease(mShaderResourceView);
	SafeRelease(mRenderTargetView);
	SafeRelease(mRenderTexture);
}
