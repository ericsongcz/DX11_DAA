/*!
 * \file IRenderable.h
 * \date 2014/07/01 15:20
 *
 * \author Song Li
 * Contact: erosnick@gmail.com
 *
 * \brief IRenderable封装了渲染单个网格所需的几何信息以及材质信息，与具体底层渲染API隔离。
 *
 * TODO: long description
 *
 * \note
*/

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
	virtual Material* getMaterial() const;
	virtual XMMATRIX getTransform();
	const vector<Vertex>& getVerticesList() const;
	const vector<UINT>& getIndicesList() const;
	int getVerticesCount() const;
	int getIndicesCount() const;
	void clear();
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