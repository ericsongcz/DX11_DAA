#pragma once

#include "resource.h"
#include "D3DApp.h"
#include "Geometry.h"

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

private:
	Geometry* mGeometry;
};