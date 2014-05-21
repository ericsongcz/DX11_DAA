#include <d3dx11.h>
#include <string>

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
	void DrawScene();

	LRESULT CALLBACK MainWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	// D3D11 stuffs.
	float mScreenWidth;
	float mScreenHeight;
	wstring mMainWinCaption;
	bool mAppPaused;
	bool mFullScreen;
	HWND mMainHWnd;
	HINSTANCE mHInstance;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
};