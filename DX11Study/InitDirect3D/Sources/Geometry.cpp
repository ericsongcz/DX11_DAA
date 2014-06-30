#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"
#include <ctime>
#include <iostream>
#include "SharedParameters.h"
#include <DirectXTex/DirectXTex.h>
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
	mMeshdata(nullptr)
{
}

void Geometry::FillMeshData(MeshData* meshData)
{
	clear();

	//mMeshdata = meshData;

	//mVertices = new Vertex[mMeshdata->verticesCount];
	//mIndices = new UINT[mMeshdata->indicesCount];
	//mVerticesCount = mMeshdata->verticesCount;
	//mIndicesCount = mMeshdata->indicesCount;

	//srand((int)time(nullptr));

	//for (int i = 0; i < mVerticesCount; i++)
	//{
	//	mVertices[i].position = mMeshdata->vertices[i];
	//	mVertices[i].color = XMFLOAT4(Colors::White);
	//	mVertices[i].normal = mMeshdata->normals[i];
	//	mVertices[i].texcoord = mMeshdata->uvs[i];
	//	mVertices[i].tangent = mMeshdata->tangents[i];
	//	mVertices[i].binormal = mMeshdata->binormals[i];
	//}

	//memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(mMeshdata->indices[0]), sizeof(UINT) * mIndicesCount);

	GeometryGenerator geometryGenerator;
	GeometryGenerator::MeshData md;
	//geometryGenerator.CreateSphere(2, 32, 32, md);
	geometryGenerator.CreateBox(4, 4, 4, md);

	mVerticesCount += md.Vertices.size();
	mVertices.resize(mVerticesCount);
	mIndicesCount += md.Indices.size();
	mIndices.resize(mIndicesCount);

	int verticesOffset = mVerticesCount - md.Vertices.size();
	int indicesOffset = mIndicesCount - md.Indices.size();

	memcpy_s(&mVertices[verticesOffset], sizeof(Vertex) * md.Vertices.size(), &(md.Vertices[0]), sizeof(Vertex) * md.Vertices.size());
	memcpy_s(&mIndices[indicesOffset], sizeof(UINT) * md.Indices.size(), &(md.Indices[0]), sizeof(UINT) * md.Indices.size());

	mMeshdata = new MeshData();

	mMeshdata->textureFiles.push_back("Fieldstone.tga");
	mMeshdata->textureFiles.push_back("FieldstoneBumpDOT3.tga");
	mMeshdata->textureFiles.push_back("blue.dds");

	RenderPackage renderPackage;
	renderPackage.diffuseTextureFile = "Fieldstone.tga";
	renderPackage.normalMapTextureFile = "FieldstoneBumpDOT3.tga";
	renderPackage.indicesCount = md.Indices.size();
	renderPackage.indicesOffset = indicesOffset;
	renderPackage.globalTransform = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	mMeshdata->renderPackages.push_back(renderPackage);

	geometryGenerator.CreateBox(16, 0.5f, 16, md);

	mVerticesCount += md.Vertices.size();
	mVertices.resize(mVerticesCount);
	mIndicesCount += md.Indices.size();
	mIndices.resize(mIndicesCount);

	verticesOffset = mVerticesCount - md.Vertices.size();
	indicesOffset = mIndicesCount - md.Indices.size();

	for (int i = 0; i < md.Indices.size(); i++)
	{
		md.Indices[i] += verticesOffset;
	}

	memcpy_s(&mVertices[verticesOffset], sizeof(Vertex) * md.Vertices.size(), &(md.Vertices[0]), sizeof(Vertex) * md.Vertices.size());
	memcpy_s(&mIndices[indicesOffset], sizeof(UINT) * md.Indices.size(), &(md.Indices[0]), sizeof(UINT) * md.Indices.size());

	renderPackage.diffuseTextureFile = "blue.dds";
	renderPackage.normalMapTextureFile = "";
	renderPackage.indicesCount = md.Indices.size();
	renderPackage.indicesOffset = indicesOffset;
	renderPackage.globalTransform = XMMatrixTranslation(0.0f, 1.5f, 0.0f);
	mMeshdata->renderPackages.push_back(renderPackage);

	map<string, ID3D11ShaderResourceView*> shaderReresourceViews;

	for (int i = 0; i < mMeshdata->textureFiles.size(); i++)
	{
		shaderReresourceViews[mMeshdata->textureFiles[i]] = CreateShaderResourceViewFromFile(mMeshdata->textureFiles[i], SharedParameters::device);
	}

	for (int i = 0; i < mMeshdata->renderPackages.size(); i++)
	{
		if (mMeshdata->renderPackages[i].diffuseTextureFile.size() > 0)
		{
			mMeshdata->renderPackages[i].diffuseTexture = shaderReresourceViews[mMeshdata->renderPackages[i].diffuseTextureFile];
			mMeshdata->renderPackages[i].hasDiffuseTexture = true;
		}

		if (mMeshdata->renderPackages[i].normalMapTextureFile.size() > 0)
		{
			mMeshdata->renderPackages[i].normalMapTexture = shaderReresourceViews[mMeshdata->renderPackages[i].normalMapTextureFile];
			mMeshdata->renderPackages[i].hasNormalMapTexture = true;
		}

		mMeshdata->renderPackages[i].RefreshTextures();
	}

	SharedParameters::renderPackages = mMeshdata->renderPackages;
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
	vertexData.pSysMem = &mVertices[0];
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
	indexData.pSysMem = &mIndices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HR(mDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer));

	setupBuffers(mDeviceContext);

	return true;
}

MeshData* Geometry::GetMeshData() const
{
	return mMeshdata;
}

void Geometry::clear()
{
	mVerticesCount = 0;
	mIndicesCount = 0;
	SafeDelete(mMeshdata);
}

void Geometry::setupBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT strides = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &strides, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
}