#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"
#include <ctime>
#include <iostream>
#include "SharedD3DDevice.h"
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

Geometry::~Geometry()
{
	clear();
}

void Geometry::FillMeshData(MeshData* meshData)
{
	clear();

	mMeshdata = meshData;
	mVertices.resize(mMeshdata->verticesCount);
	mIndices.resize(mMeshdata->indicesCount);
	mVerticesCount = mMeshdata->verticesCount;
	mIndicesCount = mMeshdata->indicesCount;

	for (int i = 0; i < mVerticesCount; i++)
	{
		mVertices[i].position = mMeshdata->vertices[i];
		mVertices[i].color = XMFLOAT4(Colors::White);
		mVertices[i].normal = mMeshdata->normals[i];
		mVertices[i].texcoord = mMeshdata->uvs[i];
		mVertices[i].tangent = mMeshdata->tangents[i];
		mVertices[i].binormal = mMeshdata->binormals[i];
	}

	memcpy_s(&mIndices[0], sizeof(UINT) * mIndicesCount, &(mMeshdata->indices[0]), sizeof(UINT) * mIndicesCount);

	//GeometryGenerator geometryGenerator;
	//GeometryGenerator::MeshData md;
	////geometryGenerator.CreateSphere(2, 32, 32, md);
	//Entity* entity = geometryGenerator.CreateBox(4, 4, 4, md);

	//Material* material = new Material();
	//material->setDiffuseTexture("Fieldstone.tga");
	//material->setNormalTexture("FieldstoneBumpDOT3.tga");

	//entity->setMaterial(material);

	//addRenderable(entity);

	//mVerticesCount += md.Vertices.size();
	//mVertices.resize(mVerticesCount);
	//mIndicesCount += md.Indices.size();
	//mIndices.resize(mIndicesCount);

	//int verticesOffset = mVerticesCount - md.Vertices.size();
	//int indicesOffset = mIndicesCount - md.Indices.size();

	//memcpy_s(&mVertices[verticesOffset], sizeof(Vertex) * md.Vertices.size(), &(md.Vertices[0]), sizeof(Vertex) * md.Vertices.size());
	//memcpy_s(&mIndices[indicesOffset], sizeof(UINT) * md.Indices.size(), &(md.Indices[0]), sizeof(UINT) * md.Indices.size());

	//mMeshdata = new MeshData();

	//mMeshdata->textureFiles.push_back("Fieldstone.tga");
	//mMeshdata->textureFiles.push_back("FieldstoneBumpDOT3.tga");
	//mMeshdata->textureFiles.push_back("blue.dds");

	//RenderPackage renderPackage;
	//renderPackage.diffuseTextureFile = "Fieldstone.tga";
	//renderPackage.normalMapTextureFile = "FieldstoneBumpDOT3.tga";
	//renderPackage.indicesCount = md.Indices.size();
	//renderPackage.indicesOffset = indicesOffset;
	//XMStoreFloat4x4(&renderPackage.globalTransform, XMMatrixTranslation(0.0f, 4.0f, 0.0f));
	//mMeshdata->renderPackages.push_back(renderPackage);

	//entity = geometryGenerator.CreateBox(16, 0.0f, 16, md);

	//material = new Material();
	//material->setDiffuseTexture("blue.dds");

	//entity->setMaterial(material);

	//addRenderable(entity);

	//prepareRenderPackages();

	//mVerticesCount += md.Vertices.size();
	//mVertices.resize(mVerticesCount);
	//mIndicesCount += md.Indices.size();
	//mIndices.resize(mIndicesCount);

	//verticesOffset = mVerticesCount - md.Vertices.size();
	//indicesOffset = mIndicesCount - md.Indices.size();

	//for (int i = 0; i < md.Indices.size(); i++)
	//{
	//	md.Indices[i] += verticesOffset;
	//}

	//memcpy_s(&mVertices[verticesOffset], sizeof(Vertex) * md.Vertices.size(), &(md.Vertices[0]), sizeof(Vertex) * md.Vertices.size());
	//memcpy_s(&mIndices[indicesOffset], sizeof(UINT) * md.Indices.size(), &(md.Indices[0]), sizeof(UINT) * md.Indices.size());

	//renderPackage.diffuseTextureFile = "blue.dds";
	//renderPackage.normalMapTextureFile = "";
	//renderPackage.indicesCount = md.Indices.size();
	//renderPackage.indicesOffset = indicesOffset;
	//XMStoreFloat4x4(&renderPackage.globalTransform, XMMatrixTranslation(0.0f, 1.5f, 0.0f));
	//mMeshdata->renderPackages.push_back(renderPackage);

	map<string, ID3D11ShaderResourceView*> shaderReresourceViews;

	for (int i = 0; i < mMeshdata->textureFiles.size(); i++)
	{
		shaderReresourceViews[mMeshdata->textureFiles[i]] = CreateShaderResourceViewFromFile(mMeshdata->textureFiles[i], SharedD3DDevice::device);
	}

	for (int i = 0; i < mMeshdata->renderPackages.size(); i++)
	{
		//if (mMeshdata->renderPackages[i].diffuseTextureFile.size() > 0)
		//{
		//	mMeshdata->renderPackages[i].diffuseTexture = shaderReresourceViews[mMeshdata->renderPackages[i].diffuseTextureFile];
		//}

		//if (mMeshdata->renderPackages[i].normalMapTextureFile.size() > 0)
		//{
		//	mMeshdata->renderPackages[i].normalMapTexture = shaderReresourceViews[mMeshdata->renderPackages[i].normalMapTextureFile];
		//}

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

	for (int i = 0; i < mRenderables.size(); i++)
	{
		SafeDelete(mRenderables[i]);
	}
}

void Geometry::setupBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT strides = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &strides, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
}

void Geometry::addRenderable(IRenderable* renderable)
{
	mRenderables.push_back(renderable);
}

void Geometry::prepareRenderPackages()
{
	for (int i = 0; i < mRenderables.size(); i++)
	{
		int verticesCount = mRenderables[i]->getVerticesCount();
		int indicesCount = mRenderables[i]->getIndicesCount();
		vector<Vertex> vertices = mRenderables[i]->getVerticesList();
		vector<UINT> indices = mRenderables[i]->getIndicesList();

		mVerticesCount += verticesCount;
		mVertices.resize(mVerticesCount);
		mIndicesCount += indicesCount;
		mIndices.resize(mIndicesCount);

		mVerticesOffset = mVerticesCount - verticesCount;
		mIndicesOffset = mIndicesCount - indicesCount;

		for (int j = 0; j < indicesCount; j++)
		{
			indices[j] += mVerticesOffset;
		}

		memcpy_s(&mVertices[mVerticesOffset], sizeof(Vertex) * verticesCount, &vertices[0], sizeof(Vertex) * verticesCount);
		memcpy_s(&mIndices[mIndicesOffset], sizeof(UINT) * indicesCount, &indices[0], sizeof(UINT) * indicesCount);

		RenderPackage renderPackage;
		XMStoreFloat4x4(&renderPackage.globalTransform, mRenderables[i]->getTransform());
		renderPackage.indicesCount = indicesCount;
		renderPackage.indicesOffset = mIndicesOffset;
		renderPackage.material = mRenderables[i]->getMaterial();
		mMeshdata->renderPackages.push_back(renderPackage);
	}
}