#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

using namespace DirectX;
using std::vector;
using std::string;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
};

struct MeshInfo
{
	MeshInfo()
	: verticesCount(0),
	  indicesCount(0)
	{
	}
	 
	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<UINT> indices;
	vector<XMFLOAT2> uvs;
	UINT verticesCount;
	UINT indicesCount;	
	string textureFileName;
	string textureFilePath;
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