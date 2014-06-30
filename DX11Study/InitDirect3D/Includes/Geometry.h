#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "GeometryGenerator.h"
#include "CommonStructures.h"

using namespace DirectX;
using std::vector;
using std::string;

class Geometry
{
public:
	Geometry();
	~Geometry();

	void FillMeshData(MeshData* meshData);
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void setupBuffers(ID3D11DeviceContext* deviceContext);
	MeshData* GetMeshData() const;
	void clear();
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11InputLayout* mInputLayout;
	int mVerticesCount;
	int mIndicesCount;
	vector<Vertex> mVertices;
	vector<UINT> mIndices;
	MeshData* mMeshdata;
};