#include "stdafx.h"
#include "Geometry.h"
#include "D3DUtils.h"

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	Vertex vertices[] = 
	{
		{ XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Red) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT4((const float*)&Colors::Green) },
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4((const float*)&Colors::Blue) }
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof (Vertex) * 3;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;

	if (HR(mDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer)))
	{
		return false;
	}

	UINT indices[] = { 0, 1, 2 };

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof (UINT) * 3;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;

	if (HR(mDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer)))
	{
		return false;
	}

	return true;
}