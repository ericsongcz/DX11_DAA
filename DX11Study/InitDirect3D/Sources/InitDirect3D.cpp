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
	fbxImporter->LoadScene("sponza.fbx");
	fbxImporter->WalkHierarchy();

	mGeometry = new Geometry();
	mGeometry->FillMeshData(fbxImporter->GetMeshInfo());

	if (!mGeometry->Initialize(SharedParameters::device, SharedParameters::deviceContext))
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
	mRenderer->beginScene();

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
	vector<RenderPackage> renderPackages = meshData->renderPackages;
	int renderPackageSize = renderPackages.size();

	for (int i = 0; i < renderPackageSize; i++)
	{
		RenderParameters renderParameters;

		worldMatrix = renderPackages[i].globalTransform;
		worldMatrix = XMMatrixMultiply(worldMatrix, mRotateMatrix);

		if (renderPackages[i].hasDiffuseTexture)
		{
			renderParameters.hasDiffuseTexture = true;
		}

		if (renderPackages[i].hasNormalMapTexture)
		{
			renderParameters.hasNormalMapTexture = true;
		}

		mRenderer->render(renderParameters, worldMatrix, mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

		if (renderPackages[i].textures.size() > 0)
		{
			mRenderer->setShaderResource(&renderPackages[i].textures[0], renderPackages[i].textures.size());
		}

		mGeometry->renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);
	}

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