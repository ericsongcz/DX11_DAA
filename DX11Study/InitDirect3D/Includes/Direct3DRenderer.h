#pragma once
#include <d3d11.h>
#include <string>
#include "GameTimer.h"
#include "Shader.h"
#include "DeferredShader.h"

using std::wstring;

class Direct3DRenderer
{
public:
	Direct3DRenderer(float width, float height, wstring title, bool fullScreen = false);
	~Direct3DRenderer();

	bool initD3D(HWND hWnd);
	void resizeBackBuffer(UINT width, UINT height);
	void beginScene();
	void endScene();
	void changeFillMode(D3D11_FILL_MODE fillMode);
	void switchFillMode();
	void render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, CXMMATRIX& viewMatrix, CXMMATRIX& projectionMatrix);
	void renderBuffer(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	void setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews);
	ID3D11Device* getDevice() const;
	void setViewport(float width, float height, float topLeftX = 0.0f, float topLeftY = 0.0f, float minDepth = 0.0f, float maxDepth = 1.0f);
	void setClearColor(int r, int g, int b);
	void renderToTexture();
private:
	// D3D11 stuffs.
	float mScreenWidth;
	float mScreenHeight;
	wstring mMainWinCaption;
	bool mAppPaused;
	bool mFullScreen;
	bool mEnable4xMsaa;
	bool mResizing;
	bool mMinimized;
	bool mMaximized;
	D3D11_FILL_MODE mFillMode;
	UINT m4xMsaaQuality;
	HWND mMainHWnd;
	HINSTANCE mHInstance;
	GameTimer mTimer;
	Shader* mShader;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;
	ID3D11RasterizerState* mSolidState;
	ID3D11RasterizerState* mWireframeState;
	float mClearColor[3];

};