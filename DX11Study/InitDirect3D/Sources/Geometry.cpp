#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"
#include <ctime>
#include <iostream>
#include "SharedParameters.h"
#include <DirectXTex/DirectXTex.h>
#include "GeometryGenerator.h"
#include <map>

using namespace DirectX;
using namespace std;

Geometry::Geometry()
	: mDevice(nullptr),
	mDeviceContext(nullptr),
	mVertexBuffer(nullptr),
	mIndexBuffer(nullptr),
	mInputLayout(nullptr),
	mVerticesCount(0),
	mIndicesCount(0),
	mVertices(nullptr),
	mIndices(nullptr),
	mMeshdata(nullptr)
{
}

void Geometry::FillMeshData(MeshData* meshData)
{
	clear();

	mMeshdata = meshData;

	mVertices = new Vertex[mMeshdata->verticesCount];
	mIndices = new UINT[mMeshdata->indicesCount];
	mVerticesCount = mMeshdata->verticesCount;
	mIndicesCount = mMeshdata->indicesCount;

	srand((int)time(nullptr));

	for (int i = 0; i < mVerticesCount; i++)
	{
		mVertices[i].position = mMeshdata->vertices[i];
		mVertices[i].color = XMFLOAT4(Colors::White);
		mVertices[i].normal = mMeshdata->normals[i];
		mVertices[i].texcoord = mMeshdata->uvs[i];
		mVertices[i].tangent = mMeshdata->tangents[i];
	}

	memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(mMeshdata->indices[0]), sizeof(UINT) * mIndicesCount);

	//GeometryGenerator geometryGenerator;
	//GeometryGenerator::MeshData meshData;
	////geometryGenerator.CreateSphere(2, 32, 32, meshData);
	//geometryGenerator.CreateBox(4, 4, 4, meshData);

	//mVerticesCount = meshData.Vertices.size();
	//mIndicesCount = meshData.Indices.size();

	//mVertices = new Vertex[mVerticesCount];
	//mIndices = new UINT[mIndicesCount];

	//for (int i = 0; i < mVerticesCount; i++)
	//{
	//	mVertices[i].position = meshData.Vertices[i].Position;
	//	mVertices[i].normal = meshData.Vertices[i].Normal;
	//	mVertices[i].texcoord = meshData.Vertices[i].TexC;
	//}

	//memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(meshData.Indices[0]), sizeof(UINT) * mIndicesCount);

	map<string, ID3D11ShaderResourceView*> shaderReresourceViews;

	for (int i = 0; i < meshData->textureFiles.size(); i++)
	{
		shaderReresourceViews[mMeshdata->textureFiles[i]] = CreateShaderResourceViewFromFile(meshData->textureFiles[i], SharedParameters::device);
	}

	for (int i = 0; i < meshData->renderPackages.size(); i ++)
	{
		if (meshData->renderPackages[i].diffuseTextureFile.size() > 0)
		{
			meshData->renderPackages[i].diffuseTexture = shaderReresourceViews[meshData->renderPackages[i].diffuseTextureFile];
			meshData->renderPackages[i].hasDiffuseTexture = true;
		}

		if (meshData->renderPackages[i].normalMapTextureFile.size() > 0)
		{
			meshData->renderPackages[i].normalMapTexture = shaderReresourceViews[meshData->renderPackages[i].normalMapTextureFile];
			meshData->renderPackages[i].hasNormalMapTexture = true;
		}

		meshData->renderPackages[i].RefreshTextures();
	}
}

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);

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

void Geometry::renderBuffer(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

MeshData* Geometry::GetMeshData() const
{
	return mMeshdata;
}

void Geometry::clear()
{
	mVerticesCount = 0;
	mIndicesCount = 0;
	SafeDelete(mVertices);
	SafeDelete(mIndices);
	SafeDelete(mMeshdata);
}
