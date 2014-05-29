#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"
#include <ctime>
#include <iostream>
#include "SharedParameters.h"
#include <DirectXTex/DirectXTex.h>

using namespace DirectX;
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
		XMVECTOR tempPosition = XMLoadFloat3(&meshInfo->vertices[i]);
		tempPosition = XMVector3Transform(tempPosition, SharedParameters::globalTransform);
		XMStoreFloat3(&meshInfo->vertices[i], tempPosition);

		mVertices[i].position = meshInfo->vertices[i];
		mVertices[i].color = XMFLOAT4(RAND_ONE_FLOAT(), RAND_ONE_FLOAT(), RAND_ONE_FLOAT(), 1.0f);
		mVertices[i].normal = meshInfo->normals[i];
		mVertices[i].texcoord = meshInfo->uvs[i];
	}

	memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(meshInfo->indices[0]), sizeof(UINT) * mIndicesCount);

	TexMetadata metaData;
	ScratchImage image;

	const char* filepath = meshInfo->textureFilePath.c_str();

	if (strlen(filepath) > 0)
	{
		size_t len = strlen(filepath) + 1;

		size_t converted = 0;

		wchar_t *WStr;

		WStr = (wchar_t*)malloc(len*sizeof(wchar_t));

		mbstowcs_s(&converted, WStr, len, filepath, _TRUNCATE);

		HR(LoadFromDDSFile(WStr, DDS_FLAGS_NONE, &metaData, image));

		ID3D11ShaderResourceView* shaderResourceView = nullptr;
		HR(CreateShaderResourceView(SharedParameters::device, image.GetImages(), image.GetImageCount(), metaData, &shaderResourceView));

		SharedParameters::shader->setShaderResource(shaderResourceView);
	}
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
	//mDeviceContext->Draw(mVerticesCount, 0);
}