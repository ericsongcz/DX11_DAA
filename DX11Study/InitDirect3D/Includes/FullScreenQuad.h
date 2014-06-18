#pragma once
#include <d3d11.h>
#include "Geometry.h"

class FullScreenQuad
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
	};
public:
	FullScreenQuad();
	~FullScreenQuad();
	bool initialize(ID3D11Device* device, int width, int height);
	void setupBuffers(ID3D11DeviceContext* deviceContext);
	vector<RenderPackage> getRenderpackge();
	void shutdown();
private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	int mVerticesCount;
	int mIndicesCount;
	vector<RenderPackage> mRenderpackage;
};