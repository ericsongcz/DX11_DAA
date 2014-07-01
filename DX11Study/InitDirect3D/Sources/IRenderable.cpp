#include "stdafx.h"
#include "IRenderable.h"
#include "D3DUtils.h"

IRenderable::IRenderable()
	: mMaterial(nullptr)
{
	XMStoreFloat4x4(&mTransform, XMMatrixIdentity());
}

IRenderable::~IRenderable()
{
	clear();
}

void IRenderable::addVertex(const Vertex& vertex)
{
	mVertices.push_back(vertex);
	mVerticesCount++;
}

void IRenderable::addIndex(const UINT index)
{
	mIndices.push_back(index);
	mIndicesCount++;
}

void IRenderable::assignVertices(const vector<Vertex>& vertices)
{
	mVertices = vertices;
	mVerticesCount = mVertices.size();
}

void IRenderable::assignIndices(const vector<UINT>& indices)
{
	mIndices = indices;
	mIndicesCount = mIndices.size();
}

void IRenderable::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	mTransform._41 = x;
	mTransform._42 = y;
	mTransform._43 = z;
}

void IRenderable::setPosition(const CXMVECTOR& position)
{
	XMStoreFloat3(&mPosition, position);
	mTransform._41 = mPosition.x;
	mTransform._42 = mPosition.y;
	mTransform._43 = mPosition.z;
}

void IRenderable::setMaterial(Material* material)
{
	mMaterial = material;
}

Material* IRenderable::getMaterial() const
{
	return mMaterial;
}

DirectX::XMMATRIX IRenderable::getTransform()
{
	return XMLoadFloat4x4(&mTransform);
}

const vector<Vertex>& IRenderable::getVerticesList() const
{
	return mVertices;
}

const vector<UINT>& IRenderable::getIndicesList() const
{
	return mIndices;
}

int IRenderable::getVerticesCount() const
{
	return mVerticesCount;
}

int IRenderable::getIndicesCount() const
{
	return mIndicesCount;
}

void IRenderable::clear()
{

}
