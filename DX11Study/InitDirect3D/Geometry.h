#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;
using std::vector;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
};

struct MeshInfo
{
	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<UINT> indices;
	UINT verticesCount;
	UINT indicesCount;
};

class Geometry
{
public:
	Geometry();
	~Geometry();

	void FillMeshData(MeshInfo* meshInfo);
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void renderBuffer();
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11InputLayout* mInputLayout;
	int mVerticesCount;
	int mIndicesCount;
	Vertex* mVertices;
	UINT* mIndices;
};