#pragma once
#include <string>
#include <vector>
#include "GeometryGenerator.h"
#include <DirectXMath.h>

using namespace DirectX;
using std::string;
using std::vector;

class IRenderable
{
	virtual RenderPackage getRenderPackage() const = 0;
protected:
	int mVerticesCount;
	int mIndicesCount;
	vector<Vertex> mVertices;
	vector<UINT> mIndices;
	RenderPackage mRenderPackage;
	string mName;
	XMMATRIX mTransform;
	XMFLOAT3 mPosition;
};