// InitDirect3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "InitDirect3D.h"
#include "D3DUtils.h"
#include <DirectXTex/DirectXTex.h>
#include "FBXImporter.h"

using namespace DirectX;

#define MAX_LOADSTRING 100

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w+t", stdout);
	freopen_s(&file, "CONIN$", "r+t", stdin);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitDirect3D* app = new InitDirect3D(800, 600, TEXT("D3DApp Demo"));
	if (app->Init())
	{
		app->Run();
	}

	delete app;

	FreeConsole();

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

	FBXImporter* fbxImporter = new FBXImporter();
	fbxImporter->Init();
	fbxImporter->LoadScene("Cube.fbx");
	fbxImporter->WalkHierarchy();

	mGeometry = new Geometry();
	mGeometry->FillMeshData(fbxImporter->GetMeshInfo());

	if (!mGeometry->Initialize(mDevice, mDeviceContext))
	{
		return false;
	}

	mShader = new Shader();
	if (!mShader->initialize(mDevice, mDeviceContext, TEXT("VertexShader.cso"), TEXT("PixelShader.cso")))
	{
		return false;
	}

	TexMetadata metaData;
	ScratchImage image;
	HR(LoadFromDDSFile(TEXT("puss.dds"), DDS_FLAGS_NONE, &metaData, image));

	HR(CreateShaderResourceView(mDevice, image.GetImages(), image.GetImageCount(), metaData, &mShaderResourceView));

	mShader->setShaderResource(mShaderResourceView);

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

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f,-2.0f));
	XMVECTOR lookAt = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4, mScreenWidth / mScreenHeight, 1.0f, 1000.0f);

	mShader->render(worldMatrix, viewMatrix, projectionMatrix);

	mGeometry->renderBuffer();

	mSwapChain->Present(0, 0);
}

void InitDirect3D::OnMouseDown(WPARAM btnState, int x, int y)
{
	int i = 100;
	int j = i * 100;
}