#pragma once
#include <d3d11.h>
#include <string>
#include "GameTimer.h"

using std::wstring;

class Direct3DRenderer
{
public:
	Direct3DRenderer(float width, float height, wstring title, bool fullScreen = false);
	~Direct3DRenderer();

	bool initD3D(HWND hWnd);
	void createDepthStencilView();
	void beginScene();
	void endScene();
	ID3D11Device* getDevice() const;
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
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;
	ID3D11RasterizerState* mSolidState;
	ID3D11RasterizerState* mWireframeState;
};