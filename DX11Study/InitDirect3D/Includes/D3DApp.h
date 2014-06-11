#include <windowsx.h>
#include <windows.h>
#include <d3d11.h>
#include <string>
#include "GameTimer.h"
#include "Direct3DRenderer.h"

using std::wstring;

class D3DApp
{
public:
	D3DApp(float width, float height, wstring title, bool fullScreen = false);
	~D3DApp();

	bool Init();
	bool InitMainWindow();
	void Run();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;

	virtual void OnResize();

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

	virtual void OnKeyDown(DWORD keyCode) {};

	void CalculateFrameStats();

	LRESULT CALLBACK MainWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	// D3D11 stuffs.
	float mScreenWidth;
	float mScreenHeight;
	wstring mMainWinCaption;
	bool mAppPaused;
	bool mResizing;
	bool mMinimized;
	bool mMaximized;
	HWND mMainHWnd;
	HINSTANCE mHInstance;
	GameTimer mTimer;
	Direct3DRenderer* mRenderer;
};