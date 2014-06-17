#include "stdafx.h"
#include "Direct3DRenderer.h"
#include "D3DUtils.h"
#include "SharedParameters.h"

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
	mDeferredBuffers(nullptr)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	SharedParameters::render = this;
}

Direct3DRenderer::~Direct3DRenderer()
{
	SafeDelete(mFullScreenQuad);
	SafeDelete(mDeferredShader);
	SafeDelete(mDeferredBuffers);
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

	SharedParameters::device = mDevice;
	SharedParameters::deviceContext = mDeviceContext;

	ID3D11Texture2D* backBuffer = nullptr;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	HR(mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));

	backBuffer->Release();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	// Initialize the description of the stencil state.
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

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
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
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
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
	mDeferredShader->initialize(mDevice, mDeviceContext, TEXT("DeferredBufferVS.cso"), TEXT("DeferredBufferPS.cso"));

	mFullScreenQuad = new FullScreenQuad();

	mLightShader = new LightShader();
	mLightShader->initialize(mDevice, mDeviceContext, TEXT("LightShaderVS.cso"), TEXT("LightShaderPS.cso"));

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
	vector<RenderPackage> renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = renderPackages[i].globalTransform;
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		if (SharedParameters::showTexture)
		{
			if (renderPackages[i].hasDiffuseTexture)
			{
				renderParameters.hasDiffuseTexture = true;
			}
		}

		if (renderPackages[i].hasNormalMapTexture)
		{
			renderParameters.hasNormalMapTexture = true;
		}

		if (renderPackages[i].textures.size() > 0)
		{
			ID3D11ShaderResourceView* color = mDeferredBuffers->getShaderResourceView(0);
			//ID3D11ShaderResourceView* normal = mDeferredBuffers->getShaderResourceView(1);
			setShaderResource(&color, 1);
			//setShaderResource(&normal, 1);
			//setShaderResource(&renderPackages[i].textures[0], renderPackages[i].textures.size());
		}

		mShader->render(renderParameters, worldMatrix, SharedParameters::camera->getViewMatrix(), SharedParameters::camera->getProjectionMatrix());

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);

		renderParameters.hasDiffuseTexture = false;
		renderParameters.hasNormalMapTexture = false;
	}
}

void Direct3DRenderer::setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews)
{
	mShader->setShaderResource(ppShaderResourceViews, numViews);
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

void Direct3DRenderer::renderToTexture(RenderParameters& renderParameters)
{
	mDeferredBuffers->setRenderTargets(mDeviceContext);
	mDeferredBuffers->clearRenderTargets(mDeviceContext, mClearColor);

	vector<RenderPackage> renderPackages = SharedParameters::renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		XMMATRIX worldMatrix = renderPackages[i].globalTransform;
		worldMatrix = XMMatrixMultiply(worldMatrix, SharedParameters::rotate);

		if (SharedParameters::showTexture)
		{
			if (renderPackages[i].hasDiffuseTexture)
			{
				renderParameters.hasDiffuseTexture = true;
			}
		}

		if (renderPackages[i].hasNormalMapTexture)
		{
			renderParameters.hasNormalMapTexture = true;
		}

		if (renderPackages[i].textures.size() > 0)
		{
			setShaderResource(&renderPackages[i].textures[0], renderPackages[i].textures.size());
		}

		mDeferredShader->render(renderParameters, worldMatrix, SharedParameters::camera->getViewMatrix(), SharedParameters::camera->getProjectionMatrix());

		renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);

		renderParameters.hasDiffuseTexture = false;
		renderParameters.hasNormalMapTexture = false;
	}

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

	vector<RenderPackage> renderPackages = mFullScreenQuad->getRenderpackge();

	renderBuffer(renderPackages[0].indicesCount, renderPackages[0].indicesOffset, 0);
}

void Direct3DRenderer::turnOnZTest(bool on)
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

void Direct3DRenderer::renderLight()
{
	//ID3D11ShaderResourceView* shaderResourceViews[] = { mDeferredBuffers->getShaderResourceView(0), mDeferredBuffers->getShaderResourceView(1) };
	ID3D11ShaderResourceView* color = CreateShaderResourceViewFromFile("Dont-understand.dds", mDevice);
	ID3D11ShaderResourceView* normal = CreateShaderResourceViewFromFile("Dont-understand.dds", mDevice);
	ID3D11ShaderResourceView* shaderResourceViews[] = { color, normal };
	mLightShader->setShaderResource(shaderResourceViews, ARRAYSIZE(shaderResourceViews));

	RenderParameters rp;
	mLightShader->render(rp, SharedParameters::camera->getWolrdMatrix(), SharedParameters::camera->getViewMatrix(), SharedParameters::camera->getOrthogonalMatrix());
}

void Direct3DRenderer::resetShaderResources()
{
	// 记得要在下次渲染前解除前面SRV的绑定，否则会报错。
	// Resource being set to OM RenderTarget slot 0 is still bound on input
	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr };
	mLightShader->setShaderResource(srvs, ARRAYSIZE(srvs));
}
