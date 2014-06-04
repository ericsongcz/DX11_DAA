#pragma once

#include "../resource.h"
#include "D3DApp.h"
#include "Geometry.h"
#include "Shader.h"
#include "Camera.h"

class InitDirect3D : public D3DApp
{
public:
	InitDirect3D(float width, float height, wstring caption);
	~InitDirect3D();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnKeyDown(DWORD keyCode);
private:
	Geometry* mGeometry;
	Shader* mShader;
	Camera* mCamera;
	ID3D11ShaderResourceView* mShaderResourceView;
	XMMATRIX mRotateMatrix;
};