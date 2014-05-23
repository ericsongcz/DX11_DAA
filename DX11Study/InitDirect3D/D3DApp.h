#include <d3dx11.h>
#include <string>
#include "GameTimer.h"

using std::wstring;

class D3DApp
{
public:
	D3DApp(float width, float height, wstring title, bool fullScreen = false);
	~D3DApp();

	bool Init();
	bool InitMainWindow();
	bool InitD3D(HWND hWnd);
	void Run();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;

	virtual void OnResize();

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

	void CalculateFrameStats();

	LRESULT CALLBACK MainWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
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