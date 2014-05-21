// InitDirect3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "InitDirect3D.h"
#include "D3DUtils.h"

#define MAX_LOADSTRING 100

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitDirect3D* app = new InitDirect3D(800, 600, TEXT("D3DApp Demo"));
	if (app->Init())
	{
		app->Run();
	}

	delete app;

	return 0;
}

InitDirect3D::InitDirect3D(float width, float height, wstring caption)
: D3DApp(width, height, caption)
{
	
}

InitDirect3D::~InitDirect3D()
{

}

bool InitDirect3D::Init()
{
	if (!D3DApp::Init())
	{
		return false;
	}

	return true;
}

void InitDirect3D::OnResize()
{
	D3DApp::OnResize();
}

void InitDirect3D::UpdateScene(float dt)
{

}

void InitDirect3D::DrawScene()
{
	float clearColor[] = { RGB256(100), RGB256(149), RGB256(237) };

	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mSwapChain->Present(0, 0);
}

void InitDirect3D::OnMouseDown(WPARAM btnState, int x, int y)
{
	int i = 100;
	int j = i * 100;
}