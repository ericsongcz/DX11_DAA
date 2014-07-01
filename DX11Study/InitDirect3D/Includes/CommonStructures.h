#pragma once

#include <d3d11.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Material.h"
#include "D3DUtils.h"

using namespace DirectX;
using std::vector;
using std::string;

struct Vertex
{
	Vertex(){}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: position(p), normal(n), tangent(t), texcoord(uv){}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: position(px, py, pz), normal(nx, ny, nz),
		tangent(tx, ty, tz), texcoord(u, v){}

	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
	XMFLOAT2 texcoord;
};

struct RenderPackage
{
	RenderPackage()
		: indicesCount(0),
		indicesOffset(0),
		ambientTexture(nullptr),
		specularTexture(nullptr),
		maskTexture(nullptr)
	{
		XMStoreFloat4x4(&globalTransform, XMMatrixIdentity());
		material = new Material();
	}

	~RenderPackage()
	{
		clear();
	}

	void RefreshTextures()
	{
		if (material->getDiffuseTexture() != nullptr)
		{
			textures.push_back(material->getDiffuseTexture());
		}

		if (material->getNormalMapTexture() != nullptr)
		{
			textures.push_back(material->getNormalMapTexture());
		}
	}

	void addTexture(ID3D11ShaderResourceView* texture)
	{
		textures.push_back(texture);
	}

	bool hasDiffuseTexture() const
	{
		return material->hasDiffuseTexture();
	}

	bool hasNormalMapTexture() const
	{
		return material->hasNormalMapTexture();
	}

	void clear()
	{
	}

	int indicesCount;
	int indicesOffset;
	string diffuseTextureFile;
	string normalMapTextureFile;
	string specularTextureFile;
	string maskTextureFile;
	XMFLOAT4X4 globalTransform;
	ID3D11ShaderResourceView* ambientTexture;
	ID3D11ShaderResourceView* specularTexture;
	ID3D11ShaderResourceView* maskTexture;
	vector<ID3D11ShaderResourceView*> textures;
	Material* material;
};

struct RenderParameters
{
	RenderParameters()
		: hasDiffuseTexture(false),
		hasNormalMapTexture(false),
		ambientColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		ambientIntensity(0.5f),
		diffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0)),
		diffuseIntensity(0.5f),
		showTexture(false)
	{}

	bool hasDiffuseTexture;
	bool hasNormalMapTexture;
	XMFLOAT4 ambientColor;
	float ambientIntensity;
	XMFLOAT4 diffuseColor;
	float diffuseIntensity;
	XMFLOAT4X4 rotate;
	XMFLOAT4X4 textureTransformMatrix;
	bool showTexture;
	XMFLOAT4 fogColor;
	float fogStart;
	float fogRange;
	float fogDensity;
	int fogType;
	int showFog;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 reflectionMatrix;
};

struct MeshData
{
	MeshData()
		: verticesCount(0),
		indicesCount(0),
		indicesOffset(0),
		meshesCount(0),
		hasTexture(false)
	{
	}

	~MeshData()
	{

	}

	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT3> tangents;
	vector<XMFLOAT3> binormals;
	vector<UINT> indices;
	vector<XMFLOAT2> uvs;
	UINT verticesCount;
	UINT indicesCount;
	vector<int> indicesOffset;
	vector<string> textureFiles;
	int meshesCount;
	bool hasTexture;
	vector<RenderPackage> renderPackages;
	vector<Material*> triangleMaterialIndices;
	vector<MaterialIdOffset*> materialIdOffsets;
};