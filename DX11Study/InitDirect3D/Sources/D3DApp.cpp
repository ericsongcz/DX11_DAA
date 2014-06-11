#include "stdafx.h"
#include "D3DApp.h"
#include "../Resource.h"
#include "D3DUtils.h"
#include <cassert>
#include <sstream>
#include "SharedParameters.h"

D3DApp* gD3DApp;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gD3DApp->MainWinProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(float width, float height, wstring title, bool fullScreen)
	:mScreenWidth(width),
	mScreenHeight(height),
	mMainWinCaption(title),
	mResizing(false),
	mMaximized(false),
	mMinimized(false),
	mAppPaused(false)
{
	gD3DApp = this;

	mRenderer = new Direct3DRenderer(width, height, title, fullScreen);
}

D3DApp::~D3DApp()
{
	SafeDelete(mRenderer);
}

bool D3DApp::Init()
{
	if (!InitMainWindow())
	{
		MessageBox(nullptr, TEXT("InitMainWindow failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	return true;
}

bool D3DApp::InitMainWindow()
{
	mHInstance = GetModuleHandle(0);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mHInstance;
	wcex.hIcon = LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D3DApp";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	mMainHWnd = CreateWindow(L"D3DApp", mMainWinCaption.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, (UINT)mScreenWidth, (UINT)mScreenHeight, NULL, NULL, mHInstance, NULL);

	if (!mMainHWnd)
	{
		return false;
	}

	if (!mRenderer->initD3D(mMainHWnd))
	{
		return false;
	}

	ShowWindow(mMainHWnd, SW_NORMAL);
	UpdateWindow(mMainHWnd);

	return true;
}



void D3DApp::Run()
{
	MSG msg;

	// 主消息循环: 
	ZeroMemory(&msg, sizeof(MSG));
	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			mTimer.Tick();
			if (!mAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

LRESULT CALLBACK D3DApp::MainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mScreenWidth = LOWORD(lParam);
		mScreenHeight = HIWORD(lParam);
		if (mRenderer->getDevice())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_KEYDOWN:
		WORD keyCode = LOWORD(wParam);

		OnKeyDown(keyCode);

		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void D3DApp::OnResize()
{
	mRenderer->createDepthStencilView();
}

void D3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWinCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mMainHWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
