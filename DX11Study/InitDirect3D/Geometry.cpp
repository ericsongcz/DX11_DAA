#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	Vertex vertices[] = 
	{
		{ XMFLOAT3(-0.5f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red), XMFLOAT2(0.0f, 0.0f)},
		{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4((const float*)&Colors::Green), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Blue), XMFLOAT2(1.0f, 1.0f) }
	};

	mVerticesCount = ARRAYSIZE(vertices);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof (Vertex) * 3;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HR(mDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer));

	UINT indices[] = { 0, 1, 2 };

	mIndicesCount = ARRAYSIZE(indices);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof (UINT) * 3;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
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