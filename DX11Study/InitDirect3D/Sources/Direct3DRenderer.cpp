#include "stdafx.h"
#include "Direct3DRenderer.h"
#include "D3DUtils.h"
#include "SharedParameters.h"
#include "SharedD3DDevice.h"

Direct3DRenderer::Direct3DRenderer(float width, float height, wstring title, bool fullScreen)
	: mScreenWidth(width),
	mScreenHeight(height),
	mMainWinCaption(title),
	mFullScreen(fullScreen),
	mEnable4xMsaa(false),
	m4xMsaaQuality(0),
	mResizing(false),
	mMaximized(false),
	mMinimized(false),
	mAppPaused(false),
	mFillMode(D3D11_FILL_SOLID),
	mShader(nullptr),
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilBuffer(nullptr),
	mDepthStencilView(nullptr),
	mSolidState(nullptr),
	mWireframeState(nullptr),
	mSamplerStatePoint(nullptr),
	mSamplerStateLinear(nullptr),
	mSamplerStateLinearClamp(nullptr),
	mSamplerStateAnisotropic(nullptr),
	mAlphaBlendState(nullptr),
	mDeferredBuffers(nullptr),
	mRenderToTexture(nullptr),
	mDepthShader(nullptr),
	mShadowMapShader(nullptr),
	mProjectiveTextureShader(nullptr)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	SharedParameters::render = this;
}

Direct3DRenderer::~Direct3DRenderer()
{
	SafeDelete(mFullScreenQuad);
	SafeDelete(mDeferredShader);
	SafeDelete(mDeferredBuffers);
	SafeRelease(mWireframeState);
	SafeRelease(mSolidState);
	SafeRelease(mAlphaBlendState);
	SafeRelease(mSamplerStateAnisotropic);
	SafeRelease(mSamplerStateLinear);
	SafeRelease(mSamplerStatePoint);
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilBuffer);
	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mDeviceContext);
	SafeRelease(mDevice);
	SafeDelete(mShader);
}

bool Direct3DRenderer::initD3D(HWND hWnd)
{
	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc.Width = (UINT)mScreenWidth;
	swapChainDesc.BufferDesc.Height = (UINT)mScreenHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = !mFullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, 3,
		D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, nullptr, &mDeviceContext));

	SharedD3DDevice::device = mDevice;
	SharedD3DDevice::deviceContext = mDeviceContext;

	ID3D11Texture2D* backBuffer = nullptr;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	HR(mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));

	backBuffer->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_DESC disabledDepthStencilDesc;

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&disabledDepthStencilDesc, sizeof(disabledDepthStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	disabledDepthStencilDesc.DepthEnable = false;
	disabledDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	disabledDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	disabledDepthStencilDesc.StencilEnable = true;
	disabledDepthStencilDesc.StencilReadMask = 0xFF;
	disabledDepthStencilDesc.StencilWriteMask = 0xFF;
	disabledDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	disabledDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	disabledDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	disabledDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(mDevice->CreateDepthStencilState(&disabledDepthStencilDesc, &mDisableDepthStencilState));

	D3D11_SAMPLER_DESC samplerPointDesc;
	ZeroMemory(&samplerPointDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerPointDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerPointDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerPointDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerPointDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerPointDesc.MipLODBias = 0.0f;
	samplerPointDesc.MaxAnisotropy = 1;
	samplerPointDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerPointDesc.BorderColor[0] = 0;
	samplerPointDesc.BorderColor[1] = 0;
	samplerPointDesc.BorderColor[2] = 0;
	samplerPointDesc.BorderColor[3] = 0;
	samplerPointDesc.MinLOD = 0;
	samplerPointDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerPointDesc, &mSamplerStatePoint));

	D3D11_SAMPLER_DESC samplerLinearDesc;
	ZeroMemory(&samplerLinearDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerLinearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerLinearDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.MipLODBias = 0.0f;
	samplerLinearDesc.MaxAnisotropy = 1;
	samplerLinearDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerLinearDesc.BorderColor[0] = 0;
	samplerLinearDesc.BorderColor[1] = 0;
	samplerLinearDesc.BorderColor[2] = 0;
	samplerLinearDesc.BorderColor[3] = 0;
	samplerLinearDesc.MinLOD = 0;
	samplerLinearDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerLinearDesc, &mSamplerStateLinear));

	D3D11_SAMPLER_DESC samplerLinearClampDesc;
	ZeroMemory(&samplerLinearClampDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerLinearClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerLinearClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearClampDesc.MipLODBias = 0.0f;
	samplerLinearClampDesc.MaxAnisotropy = 1;
	samplerLinearClampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerLinearClampDesc.BorderColor[0] = 0;
	samplerLinearClampDesc.BorderColor[1] = 0;
	samplerLinearClampDesc.BorderColor[2] = 0;
	samplerLinearClampDesc.BorderColor[3] = 0;
	samplerLinearClampDesc.MinLOD = 0;
	samplerLinearClampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerLinearClampDesc, &mSamplerStateLinearClamp));

	D3D11_SAMPLER_DESC samplerAnisotropicDesc;
	ZeroMemory(&samplerLinearDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerAnisotropicDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerAnisotropicDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.MipLODBias = 0.0f;
	samplerAnisotropicDesc.MaxAnisotropy = 16;
	samplerAnisotropicDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerAnisotropicDesc.BorderColor[0] = 0;
	samplerAnisotropicDesc.BorderColor[1] = 0;
	samplerAnisotropicDesc.BorderColor[2] = 0;
	samplerAnisotropicDesc.BorderColor[3] = 0;
	samplerAnisotropicDesc.MinLOD = 0;
	samplerAnisotropicDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerAnisotropicDesc, &mSamplerStateAnisotropic));

	D3D11_BLEND_DESC alphaBlendStateDesc;
	ZeroMemory(&alphaBlendStateDesc, sizeof(D3D11_BLEND_DESC));

	alphaBlendStateDesc.AlphaToCoverageEnable = false;
	alphaBlendStateDesc.IndependentBlendEnable = false;
	alphaBlendStateDesc.RenderTarget[0].BlendEnable = true;
	alphaBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(mDevice->CreateBlendState(&alphaBlendStateDesc, &mAlphaBlendState));

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
#if USE_RIGHT_HAND
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
#else
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
#endif // USE_RIGHT_HAND

	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FrontCounterClockwise = false;

	HR(mDevice->CreateRasterizerState(&rasterizerDesc, &mSolidState));

	mDeviceContext->RSSetState(mSolidState);

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
#if USE_RIGHT_HAND
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
#else
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
#endif // USE_RIGHT_HAND
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FrontCounterClockwise = false;
	HR(mDevice->CreateRasterizerState(&rasterizerDesc, &mWireframeState));

	mShader = new Shader();
	if (!mShader->initialize(mDevice, mDeviceContext, TEXT("VertexShader.cso"), TEXT("PixelShader.cso")))
	{
		return false;
	}

	mDeferredBuffers = new DeferredBuffers();

	mDeferredShader = new DeferredShader();
	mDeferredShader->initialize(mDevice, mDeviceContext, TEXT("DeferredShaderVS.cso"), TEXT("DeferredShaderPS.cso"));

	mFullScreenQuad = new FullScreenQuad();

	mLightShader = new LightShader();
	mLightShader->initialize(mDevice, mDeviceContext, TEXT("LightShaderVS.cso"), TEXT("LightShaderPS.cso"));

	mRenderToTexture = new RenderToTexture();

	mReflectionShader = new ReflectionShader();
	mReflectionShader->initialize(mDevice, mDeviceContext, TEXT("ReflectionShaderVS.cso"), TEXT("ReflectionShaderPS.cso"));

	mProjectiveTextureShader = new ProjectiveTextureShader();
	mProjectiveTextureShader->initialize(mDevice, mDeviceContext, TEXT("ProjectiveTextureVS.cso"), TEXT("ProjectiveTexturePS.cso"));

	mProjectiveTexture = CreateShaderResourceViewFromFile("dx11.dds", mDevice);

	mDepthTexture = new RenderToTexture();

	mDepthShader = new DepthShader();
	mDepthShader->initialize(mDevice, mDeviceContext, TEXT("DepthShaderVS.cso"), TEXT("DepthShaderPS.cso"));

	mShadowMapShader = new ShadowMapShader();
	mShadowMapShader->initialize(mDevice, mDeviceContext, TEXT("ShadowMapVS.cso"), TEXT("ShadowMapPS.cso"));

	mViewPoint.setPosition(0.0f, 5.0f, 20.0f);
	mViewPoint.lookAt(0.0f, 0.0f, -1.0f);
	mViewPoint.setProjectionParameters(XM_PI / 4.0f, 1.0f, SCREEN_NEAR, SCREEN_DEPTH);

	mLight.setPosition(500.0f, 500.0f, 0.0f);
	mLight.lookAt(-1.0f, -1.0f, 0.0f);
	mLight.setProjectionParameters(XM_PI / 2.0f, 1.0f, SCREEN_NEAR, SCREEN_DEPTH);
	mLight.setOrthogonalParameters(100.0f, SCREEN_NEAR, SCREEN_DEPTH);

	setClearColor(100, 149, 237);

	return true;
}

void Direct3DRenderer::resizeBackBuffer(UINT width, UINT height)
{
	mScreenWidth = width;
	mScreenHeight = height;

	assert(mDeviceContext);
	assert(mDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	SafeRelease(mRenderTargetView);
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilBuffer);

	// Resize the swap chain and recreate the render target view.
	HR(mSwapChain->ResizeBuffers(1, (UINT)mScreenWidth, (UINT)mScreenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	SafeRelease(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = (UINT)mScreenWidth;
	depthStencilDesc.Height = (UINT)mScreenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	resetRenderTarget();

	mDeferredBuffers->initialize(mDevice, mScreenWidth, mScreenHeight, 1.0f, 0.0f);
	mFullScreenQuad->initialize(mDevice, mScreenWidth, mScreenHeight);
	mRenderToTexture->initialize(mDevice, mScreenWidth, mScreenHeight);
	mDepthTexture->initialize(mDevice, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
}

ID3D11Device* Direct3DRenderer::getDevice() const
{
	return mDevice;
}

void Direct3DRenderer::beginScene()
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, mClearColor);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Direct3DRenderer::endScene()
{
	mSwapChain->Present(0, 0);
}

void Direct3DRenderer::changeFillMode(D3D11_FILL_MODE fillMode)
{
	if (fillMode == D3D11_FILL_SOLID)
	{
		mDeviceContext->RSSetState(mSolidState);
	}
	else
	{
		mDeviceContext->RSSetState(mWireframeState);
	}
}

void Direct3DRenderer::switchFillMode()
{
	if (mFillMode == D3D11_FILL_SOLID)
	{
		mFillMode = D3D11_FILL_WIREFRAME;
		mDeviceContext->RSSetState(mWireframeState);
	}
	else
	{
		mFillMode = D3D11_FILL_SOLID;
		mDeviceContext->RSSetState(mSolidState);
	}
}

void Direct3DRenderer::render(RenderParameters& renderParameters)
{
	vector<RenderPackage>& renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		if (SharedParameters::showTexture)
		{
			if (renderPackages[i].hasDiffuseTexture())
			{
				renderParameters.hasDiffuseTexture = true;
			}
		}

		if (renderPackages[i].hasNormalMapTexture())
		{
			renderParameters.hasNormalMapTexture = true;
		}

		if (renderPackages[i].textures.size() > 0)
		{
			setShaderResource(0, renderPackages[i].textures.size(), &renderPackages[i].textures[0]);
		}

		XMStoreFloat4(&renderParameters.pointLightPosition, mLight.getPosition());
		mShader->render(renderParameters, worldMatrix, XMLoadFloat4x4(&renderParameters.viewMatrix), XMLoadFloat4x4(&renderParameters.projectionMatrix));

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);

		renderParameters.hasDiffuseTexture = false;
		renderParameters.hasNormalMapTexture = false;
	}
}

void Direct3DRenderer::setShaderResource(UINT startSlot, UINT numViews, ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	mDeviceContext->PSSetShaderResources(startSlot, numViews, ppShaderResourceViews);
}

void Direct3DRenderer::setViewport(float width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth)
{
	// Set the viewport transform.
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
	viewport.TopLeftX = topLeftX;
	viewport.TopLeftY = topLeftY;

	mDeviceContext->RSSetViewports(1, &viewport);
}

void Direct3DRenderer::setClearColor(int r, int g, int b)
{
	mClearColor[0] = RGB256(r);
	mClearColor[1] = RGB256(g);
	mClearColor[2] = RGB256(b);
}

void Direct3DRenderer::renderToDeferredBuffers(RenderParameters& renderParameters)
{
	mDeferredBuffers->setRenderTargets(mDeviceContext);
	mDeferredBuffers->clearRenderTargets(mDeviceContext, mClearColor);

	vector<RenderPackage> renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		if (SharedParameters::showTexture)
		{
			if (renderPackages[i].hasDiffuseTexture())
			{
				renderParameters.hasDiffuseTexture = true;
			}
		}

		if (renderPackages[i].hasNormalMapTexture())
		{
			renderParameters.hasNormalMapTexture = true;
		}

		if (renderPackages[i].textures.size() > 0)
		{
			setShaderResource(0, renderPackages[i].textures.size(), &renderPackages[i].textures[0]);
		}

		mDeferredShader->render(renderParameters, worldMatrix, SharedParameters::camera->getViewMatrix(), SharedParameters::camera->getProjectionMatrix());

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);

		renderParameters.hasDiffuseTexture = false;
		renderParameters.hasNormalMapTexture = false;
	}

	resetRenderTarget();
}


void Direct3DRenderer::renderToTexture(RenderParameters& renderParameters)
{
	mRenderToTexture->setRenderTarget(mDeviceContext);
	mRenderToTexture->clearRenderTarget(mDeviceContext, mClearColor);

	render(renderParameters);

	resetRenderTarget();
}

void Direct3DRenderer::renderBuffer(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	mDeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void Direct3DRenderer::resetRenderTarget()
{
	// Bind the render target view and depth/stencil view to the pipeline.
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	setViewport(mScreenWidth, mScreenHeight);
}

void Direct3DRenderer::renderQuad(RenderParameters& renderParameters)
{
	mFullScreenQuad->setupBuffers(mDeviceContext);

	vector<RenderPackage*> renderPackages = mFullScreenQuad->getRenderpackge();

	renderBuffer(renderPackages[0]->indicesCount, renderPackages[0]->indicesOffset, 0);
}

void Direct3DRenderer::enableOnZTest(bool on)
{
	if (on)
	{
		mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
	}
	else
	{
		mDeviceContext->OMSetDepthStencilState(mDisableDepthStencilState, 1);
	}
}

void Direct3DRenderer::renderLight(RenderParameters& renderParameters)
{
	ID3D11ShaderResourceView* shaderResourceViews[] = { mDeferredBuffers->getShaderResourceView(0),
		mDeferredBuffers->getShaderResourceView(1),
		mDeferredBuffers->getShaderResourceView(2) };
	setShaderResource(0, ARRAYSIZE(shaderResourceViews), shaderResourceViews);

	// 全屏的Quad世界坐标位置始终不变，也不需要根据摄像机的变换而变换，另外因为坐标已经是投影后的尺寸，也不需要经过投影变换。
	// 所以绘制Quad时候三个矩阵只需要传入单位矩阵即可。
	mLightShader->render(renderParameters, XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity());
}

void Direct3DRenderer::renderReflection(RenderParameters& renderParameters)
{
	vector<RenderPackage>& renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		renderPackages[i].addTexture(mRenderToTexture->getShaderResourceView());

		if (renderPackages[i].textures.size() > 0)
		{
			setShaderResource(0, renderPackages[i].textures.size(), &renderPackages[i].textures[0]);
		}

		mReflectionShader->render(renderParameters, worldMatrix, XMLoadFloat4x4(&renderParameters.viewMatrix), XMLoadFloat4x4(&renderParameters.projectionMatrix));

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);
	}
}

void Direct3DRenderer::renderProjectiveTexture(RenderParameters& renderParameters)
{
	vector<RenderPackage>& renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		setShaderResource(0, 1, &mProjectiveTexture);

		XMStoreFloat4x4(&renderParameters.viewMatrix2, mViewPoint.getViewMatrix());
		XMStoreFloat4x4(&renderParameters.projectionMatrix2, mViewPoint.getProjectionMatrix());
		XMStoreFloat4x4(&renderParameters.projectionMatrix2, mViewPoint.getOrthogonalMatrix());
		mProjectiveTextureShader->render(renderParameters, worldMatrix, XMLoadFloat4x4(&renderParameters.viewMatrix), XMLoadFloat4x4(&renderParameters.projectionMatrix));

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);
	}
}

void Direct3DRenderer::renderDepth(RenderParameters& renderParameters)
{
	mDepthTexture->setRenderTarget(mDeviceContext);
	mDepthTexture->clearRenderTarget(mDeviceContext, mClearColor);

	vector<RenderPackage>& renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		XMStoreFloat4x4(&renderParameters.lightViewMatrix, mLight.getViewMatrix());
		XMStoreFloat4x4(&renderParameters.lightProjectionMatrix, mLight.getProjectionMatrix());
		XMStoreFloat4x4(&renderParameters.lightProjectionMatrix, mLight.getOrthogonalMatrix());
		mDepthShader->render(renderParameters, worldMatrix, XMLoadFloat4x4(&renderParameters.lightViewMatrix), XMLoadFloat4x4(&renderParameters.lightProjectionMatrix));

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);
	}

	resetRenderTarget();
}

void Direct3DRenderer::renderShadowMap(RenderParameters& renderParameters)
{
	vector<RenderPackage>& renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = XMLoadFloat4x4(&renderPackages[i].globalTransform);
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		if (SharedParameters::showTexture)
		{
			if (renderPackages[i].hasDiffuseTexture())
			{
				renderParameters.hasDiffuseTexture = true;
			}
		}

		if (renderPackages[i].hasNormalMapTexture())
		{
			renderParameters.hasNormalMapTexture = true;
		}

		if (renderPackages[i].textures.size() > 0)
		{
			setShaderResource(0, renderPackages[i].textures.size(), &renderPackages[i].textures[0]);
		}

		ID3D11ShaderResourceView* srv = mDepthTexture->getShaderResourceView();
		setShaderResource(2, 1, &srv);
		
		ID3D11SamplerState* samplers[] = { mSamplerStateLinear, mSamplerStateLinearClamp };
		setSamplerStates(0, 2, samplers);

		XMStoreFloat4(&renderParameters.pointLightPosition, mLight.getPosition());
		XMStoreFloat4x4(&renderParameters.lightViewMatrix, mLight.getViewMatrix());
		XMStoreFloat4x4(&renderParameters.lightProjectionMatrix, mLight.getProjectionMatrix());
		XMStoreFloat4x4(&renderParameters.lightProjectionMatrix, mLight.getOrthogonalMatrix());
		mShadowMapShader->render(renderParameters, worldMatrix, XMLoadFloat4x4(&renderParameters.viewMatrix), XMLoadFloat4x4(&renderParameters.projectionMatrix));

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);

		renderParameters.hasDiffuseTexture = false;
		renderParameters.hasNormalMapTexture = false;
	}
}

void Direct3DRenderer::resetShaderResources()
{
	// 记得要在下次渲染前解除前面SRV的绑定，否则会报错。
	// Resource being set to OM RenderTarget slot 0 is still bound on input.
	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr, nullptr };
	setShaderResource(0, ARRAYSIZE(srvs), srvs);
}

void Direct3DRenderer::enableAlphaBlend(bool enable)
{
	if (enable)
	{
		mDeviceContext->OMSetBlendState(mAlphaBlendState, nullptr, 0);
	}
	else
	{
		mDeviceContext->OMSetBlendState(nullptr, nullptr, 0);
	}
}

void Direct3DRenderer::setSamplerState(UINT startSlot, UINT numSamplers, ESamplerType samplerType)
{
	switch (samplerType)
	{
	case ST_POINT:
		mDeviceContext->PSSetSamplers(startSlot, numSamplers, &mSamplerStatePoint);
		break;

	case ST_LINEAR:
		mDeviceContext->PSSetSamplers(startSlot, numSamplers, &mSamplerStateLinear);
		break;

	case ST_ANISOTROPIC:
		mDeviceContext->PSSetSamplers(startSlot, numSamplers, &mSamplerStateAnisotropic);
		break;

	default:
		break;
	}
}

void Direct3DRenderer::setSamplerStates(UINT startSlot, UINT numSamplers, ID3D11SamplerState* const* samplers)
{
	mDeviceContext->PSSetSamplers(startSlot, numSamplers, samplers);
}
