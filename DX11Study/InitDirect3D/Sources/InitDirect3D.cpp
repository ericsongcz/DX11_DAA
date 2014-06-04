// InitDirect3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "InitDirect3D.h"
#include "D3DUtils.h"
#include <DirectXTex/DirectXTex.h>
#include "FBXImporter.h"
#include "SharedParameters.h"

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
	fbxImporter->LoadScene("elephant.fbx");
	fbxImporter->WalkHierarchy();

	mShader = new Shader();
	if (!mShader->initialize(mDevice, mDeviceContext, TEXT("VertexShader.cso"), TEXT("PixelShader.cso")))
	{
		return false;
	}

	mGeometry = new Geometry();
	mGeometry->FillMeshData(fbxImporter->GetMeshInfo());

	if (!mGeometry->Initialize(mDevice, mDeviceContext))
	{
		return false;
	}

	mCamera = new Camera();
	mCamera->setAspectRatio(mScreenWidth / mScreenHeight);

	mRotateMatrix = XMMatrixIdentity();

	return true;
}

void InitDirect3D::OnResize()
{
	D3DApp::OnResize();

	if (mCamera != nullptr)
	{
		mCamera->setAspectRatio(mScreenWidth / mScreenHeight);
	}
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
	
	worldMatrix = XMMatrixMultiply(worldMatrix, mRotateMatrix);
#if USE_RIGHT_HAND
	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 10.0f));
#else
	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, -10.0f));
#endif
	XMVECTOR lookAt = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));

#if USE_RIGHT_HAND
	XMMATRIX viewMatrix = XMMatrixLookAtRH(eye, lookAt, up);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(XM_PI / 4, mScreenWidth / mScreenHeight, 1.0f, 1000.0f);
#else
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4, mScreenWidth / mScreenHeight, 1.0f, 1000.0f);
#endif

	MeshData* meshData = mGeometry->GetMeshData();
	int meshCount = meshData->meshesCount;
	int indicesCount = 0;
	int indicesOffset = 0;

	for (int i = 0; i < meshCount; i++)
	{
		worldMatrix = meshData->globalTransforms[i];
		worldMatrix = XMMatrixMultiply(worldMatrix, mRotateMatrix);
		mShader->render(worldMatrix, mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

		indicesCount = meshData->indicesCounts[i];
		indicesOffset = meshData->indicesOffset[i];

		mGeometry->renderBuffer(indicesCount, indicesOffset, 0);
	}

	mSwapChain->Present(0, 0);
}

void InitDirect3D::OnMouseDown(WPARAM btnState, int x, int y)
{
	int i = 100;
	int j = i * 100;
}

void InitDirect3D::OnKeyDown(DWORD keyCode)
{
	float time = mTimer.DeltaTime();

	switch (keyCode)
	{
	case VK_ESCAPE:
		DestroyWindow(mMainHWnd);

		break;
	case VK_UP:
	{
		static float rotate = 0.0f;

		rotate += time * 720.0f;

		mRotateMatrix = RotationX(rotate);
	}

		break;
	case VK_W:
		mCamera->fly(100.0f * time);

		break;
	case VK_DOWN:
	{
		static float rotate = 0.0f;

		rotate += time * 100.0f;

		mRotateMatrix = RotationX(-rotate);
	}

		break;
	case VK_S:
		mCamera->fly(-100.0f * time);

		break;
	case VK_LEFT:
	{
		static float rotate = 0.0f;

		rotate += time * 100.0f;

		mRotateMatrix = RotationY(rotate);
	}

		break;
	case VK_A:
		mCamera->strafe(-100.0f * time);

		break;
	case VK_RIGHT:
	{
		static float rotate = 0.0f;

		rotate += time * 100.0f;

		mRotateMatrix = RotationX(-rotate);
	}

		break;
	case VK_D:
		mCamera->strafe(100.0f * time);
		break;

	case VK_Q:
		mCamera->walk(100.0f * time);
		break;

	case VK_E:
		mCamera->walk(-100.0f * time);
		break;

	case VK_F:
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
		break;
	}
}