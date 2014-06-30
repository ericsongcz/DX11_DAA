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

	return true;
}

void RenderToTexture::setRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, depthStencilView);
}

void RenderToTexture::clearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float clearColor[4])
{
	deviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
