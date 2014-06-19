#include "stdafx.h"
#include "FullScreenQuad.h"
#include "D3DUtils.h"

FullScreenQuad::FullScreenQuad()
	:mVertexBuffer(nullptr),
	 mIndexBuffer(nullptr),
	 mVerticesCount(0),
	 mIndicesCount(0)
{
}

FullScreenQuad::~FullScreenQuad()
{
	shutdown();
}

vector<RenderPackage> FullScreenQuad::getRenderpackge()
{
	return mRenderpackage;
}

bool FullScreenQuad::initialize(ID3D11Device* device, int width, int height)
{
	shutdown();
	mRenderpackage.clear();

	float left = ((float)width / 2) * -1;
	float right = left + (float)width;
	float top = (float)height / 2;
	float bottom = top - (float)height;

	left /= 2;
	right /= 2;
	top /= 2;
	bottom /= 2;

	mVerticesCount = 4;

	Vertex* vertices = new Vertex[mVerticesCount];

	vertices[0].position = XMFLOAT3(left, top, 0.0f);
	vertices[0].texcoord = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, top, 0.0f);
	vertices[1].texcoord = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[2].texcoord = XMFLOAT2(1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[3].texcoord = XMFLOAT2(0.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA verticesData;
	ZeroMemory(&verticesData, sizeof(D3D11_SUBRESOURCE_DATA));

	verticesData.pSysMem = vertices;
	verticesData.SysMemPitch = 0;
	verticesData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&vertexBufferDesc, &verticesData, &mVertexBuffer));

	delete[] vertices;

	mIndicesCount = 6;

	ULONG indices[] = { 0, 3, 2, 0, 2, 1 };

	D3D11_BUFFER_DESC indicesBufferDesc;
	ZeroMemory(&indicesBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indicesBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indicesBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indicesBufferDesc.ByteWidth = sizeof(ULONG) * 6;
	indicesBufferDesc.StructureByteStride = 0;
	indicesBufferDesc.CPUAccessFlags = 0;
	indicesBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indicesData;
	ZeroMemory(&indicesData, sizeof(D3D11_SUBRESOURCE_DATA));

	indicesData.pSysMem = indices;

	HR(device->CreateBuffer(&indicesBufferDesc, &indicesData, &mIndexBuffer));

	RenderPackage rp;
	rp.indicesCount = 6;
	rp.indicesOffset = 0;

	mRenderpackage.push_back(rp);

	return true;
}

void FullScreenQuad::setupBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FullScreenQuad::shutdown()
{
	SafeRelease(mIndexBuffer);
	SafeRelease(mVertexBuffer);
}
