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
	//mVertices(nullptr),
	mIndices(nullptr),
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
	//geometryGenerator.CreateSphere(2, 32, 32, meshData);
	geometryGenerator.CreateBox(4, 4, 4, md);

	mVerticesCount = md.Vertices.size();
	mIndicesCount = md.Indices.size();

	//mVertices = new Vertex[mVerticesCount];
	mIndices = new UINT[mIndicesCount];

	//for (int i = 0; i < mVerticesCount; i++)
	//{
	//	mVertices[i].position = md.Vertices[i].Position;
	//	mVertices[i].normal = md.Vertices[i].Normal;
	//	mVertices[i].tangent = md.Vertices[i].TangentU;
	//	mVertices[i].texcoord = md.Vertices[i].TexC;
	//}

	Vertex vertex;
	for (int i = 0; i < mVerticesCount; i++)
	{
		vertex.position = md.Vertices[i].Position;
		vertex.normal = md.Vertices[i].Normal;
		vertex.tangent = md.Vertices[i].TangentU;
		vertex.texcoord = md.Vertices[i].TexC;
		mVertices.push_back(vertex);
	}

	memcpy_s(mIndices, sizeof(UINT) * mIndicesCount, &(md.Indices[0]), sizeof(UINT) * mIndicesCount);

	mMeshdata = new MeshData();

	mMeshdata->textureFiles.push_back("Fieldstone.tga");
	mMeshdata->textureFiles.push_back("FieldstoneBumpDOT3.tga");

	RenderPackage renderPackage;
	renderPackage.diffuseTextureFile = "Fieldstone.tga";
	renderPackage.normalMapTextureFile = "FieldstoneBumpDOT3.tga";
	renderPackage.indicesCount = mIndicesCount;
	renderPackage.indicesOffset = 0;
	renderPackage.globalTransform = XMMatrixTranslation(-5.0f, 0.0, 0.0f);
	mMeshdata->renderPackages.push_back(renderPackage);

	map<string, ID3D11ShaderResourceView*> shaderReresourceViews;

	for (int i = 0; i < mMeshdata->textureFiles.size(); i++)
	{
		shaderReresourceViews[mMeshdata->textureFiles[i]] = CreateShaderResourceViewFromFile(mMeshdata->textureFiles[i], SharedParameters::device);
	}

	for (int i = 0; i < mMeshdata->renderPackages.size(); i++)
	{
		if (meshData->renderPackages[i].diffuseTextureFile.size() > 0)
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
	indexData.pSysMem = mIndices;
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
	//SafeDelete(mVertices);
	SafeDelete(mIndices);
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
