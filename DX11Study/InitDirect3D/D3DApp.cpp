#include "stdafx.h"
#include "D3DApp.h"
#include <windows.h>
#include "Resource.h"
#include "Utils.h"

D3DApp* gD3DApp;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gD3DApp->MainWinProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(float width, float height, wstring title, bool fullScreen)
	:mScreenWidth(width),
	mScreenHeight(height),
	mMainWinCaption(title),
	mFullScreen(fullScreen),
	mAppPaused(false),
	mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilBuffer(nullptr),
	mDepthStencilView(nullptr)
{
	gD3DApp = this;
}

D3DApp::~D3DApp()
{
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilBuffer);
	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mDeviceContext);
	SafeRelease(mDevice);
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
		CW_USEDEFAULT, 0, mScreenWidth, mScreenHeight, NULL, NULL, mHInstance, NULL);

	if (!mMainHWnd)
	{
		return false;
	}

	if (!InitD3D(mMainHWnd))
	{
		return false;
	}

	ShowWindow(mMainHWnd, SW_NORMAL);
	UpdateWindow(mMainHWnd);

	return true;
}

bool D3DApp::InitD3D(HWND hWnd)
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

	swapChainDesc.BufferDesc.Width = 800;
	swapChainDesc.BufferDesc.Height = 600;
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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, 3,
		D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, nullptr, &mDeviceContext);

	if (FAILED(hr))
	{
		MessageBox(hWnd, TEXT("D3D11CreateDeviceAndSwapChain failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	hr = mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);

	if (FAILED(hr))
	{
		MessageBox(hWnd, TEXT("CreateRenderTargetView failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	backBuffer->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilDesc.Width = 800;
	depthStencilDesc.Height = 600;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);

	if (FAILED(hr))
	{
		MessageBox(hWnd, TEXT("CreateTexture2D failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);

	if (FAILED(hr))
	{
		MessageBox(hWnd, TEXT("CreateDepthStencilView failed!"), TEXT("Error"), MB_OK);
		return false;
	}

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	D3D11_VIEWPORT viewport;
	viewport.Width = 800.0f;
	viewport.Height = 600.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	mDeviceContext->RSSetViewports(1, &viewport);

	return true;
}

void D3DApp::Run()
{
	MSG msg;

	// 主消息循环: 
	ZeroMemory(&msg, sizeof(MSG));

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
			if (!mAppPaused)
			{
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

void D3DApp::DrawScene()
{
	float clearColor[] = { RGB256(100), RGB256(149), RGB256(237) };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mSwapChain->Present(0, 0);
}

LRESULT CALLBACK D3DApp::MainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
		}
		else
		{
			mAppPaused = false;
		}
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}