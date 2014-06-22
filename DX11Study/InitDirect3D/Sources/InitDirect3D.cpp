// InitDirect3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "InitDirect3D.h"
#include "D3DUtils.h"
#include <DirectXTex/DirectXTex.h>
#include "FBXImporter.h"
#include "SharedParameters.h"
#include "Direct3DRenderer.h"

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

	InitDirect3D* app = new InitDirect3D(700, 555, TEXT("D3DApp Demo"));
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
	fbxImporter->LoadScene("NormalMap2.fbx");
	fbxImporter->WalkHierarchy();

	mGeometry = new Geometry();
	mGeometry->FillMeshData(fbxImporter->GetMeshInfo());

	if (!mGeometry->Initialize(SharedParameters::device, SharedParameters::deviceContext))
	{
		return false;
	}

	mCamera = new Camera();
	mCamera->setAspectRatio(mScreenWidth / mScreenHeight);
	mCamera->setPosition(XMLoadFloat3(&XMFLOAT3(0.0f, 5.0f, 20.0f)));

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
	mRenderer->beginScene();

	SharedParameters::showTexture = true;

	RenderParameters renderParameters;
	renderParameters.ambientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0);
	renderParameters.diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0);;
	renderParameters.ambientIntensity = 0.5f;
	renderParameters.diffuseIntensity = 0.5f;

	mRenderer->render(renderParameters);

	mRenderer->endScene();
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
	case VK_W:
		mCamera->fly(100.0f * time);

		break;
	case VK_S:
		mCamera->fly(-100.0f * time);

		break;
	case VK_A:
		mCamera->strafe(-100.0f * time);

		break;
	case VK_D:
		mCamera->strafe(200.0f * time);

		break;
	case VK_Q:
		mCamera->walk(200.0f * time);

		break;
	case VK_E:
		mCamera->walk(-200.0f * time);

		break;
	case VK_F:
		mRenderer->switchFillMode();

		break;
	case VK_UP:
	{
		mCamera->pitch(time * -100.0f);
	}

		break;
	case VK_DOWN:
	{
		mCamera->pitch(time * 100.0f);
	}

		break;
	case VK_LEFT:
		mCamera->yaw(100.0f * time);

		break;
	case VK_RIGHT:
		mCamera->yaw(-100.0f * time);

		break;
	}
}