#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "CommonStructures.h"

using namespace DirectX;
using std::string;
using std::vector;

class IRenderable
{
public:
	IRenderable();
	virtual ~IRenderable();
	virtual void addVertex(const Vertex& vertex);
	virtual void addIndex(const UINT index);
	virtual void assignVertices(const vector<Vertex>& vertices);
	virtual void assignIndices(const vector<UINT>& indices);
	virtual void setPosition(float x, float y, float z);
	virtual void setPosition(const CXMVECTOR& position);
	virtual void setMaterial(Material* material);
	virtual XMMATRIX getTransform();
	const vector<Vertex>& getVerticesList() const;
	const vector<UINT>& getIndicesList() const;
	int getVerticesCount() const;
	int getIndicesCount() const;
protected:
	int mVerticesCount;
	int mIndicesCount;
	vector<Vertex> mVertices;
	vector<UINT> mIndices;
	string mName;
	XMFLOAT4X4 mTransform;
	XMFLOAT3 mPosition;
	Material* mMaterial;
};