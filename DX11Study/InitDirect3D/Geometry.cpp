#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"
#include <ctime>
#include <iostream>

using namespace std;

Geometry::Geometry()
{
}

void Geometry::FillMeshData(MeshInfo* meshInfo)
{
	mVertices = new Vertex[meshInfo->verticesCount];
	mIndices = new UINT[meshInfo->indicesCount];
	mVerticesCount = meshInfo->verticesCount;
	mIndicesCount = meshInfo->indicesCount;

	srand((int)time(nullptr));

	for (int i = 0; i < mVerticesCount; i++)
	{
		mVertices[i].position = meshInfo->vertices[i];
		mVertices[i].color = XMFLOAT4(RAND_ONE_FLOAT(), RAND_ONE_FLOAT(), RAND_ONE_FLOAT(), 1.0f);

		Log("(%f, %f, %f)\n", meshInfo->vertices[i].x, meshInfo->vertices[i].y, meshInfo->vertices[i].z);
	}

	memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(meshInfo->indices[0]), sizeof(UINT) * mIndicesCount);
}

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof (Vertex) * mVerticesCount;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = mVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HR(mDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer));

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof (UINT) * mIndicesCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = mIndices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HR(mDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer));

	UINT strides = sizeof(Vertex);
	UINT offset = 0;

	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &strides, &offset);
	mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);

	return true;
}

void Geometry::renderBuffer()
{
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->DrawIndexed(mIndicesCount, 0, 0);
}