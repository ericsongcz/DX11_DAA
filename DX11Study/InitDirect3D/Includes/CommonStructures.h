#pragma once

#include <d3d11.h>
#include <vector>

using std::vector;

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


struct Material
{
	Material() {}
	Material(int id, string diffuse, string normalMap)
		: materialId(id),
		diffuseTextureFile(diffuse),
		normalMapTextureFile(normalMap)
	{}

	int materialId;
	string diffuseTextureFile;
	string normalMapTextureFile;
};

struct MaterialIdOffset
{
	MaterialIdOffset()
		: polygonCount(0)
	{}
	int polygonCount;
	Material material;
};

struct RenderPackage
{
	RenderPackage()
		: indicesCount(0),
		indicesOffset(0),
		hasDiffuseTexture(false),
		hasNormalMapTexture(false),
		ambientTexture(nullptr),
		diffuseTexture(nullptr),
		normalMapTexture(nullptr),
		specularTexture(nullptr),
		maskTexture(nullptr),
		globalTransform(XMMatrixIdentity())
	{}

	void RefreshTextures()
	{
		if (diffuseTexture != nullptr)
		{
			textures.push_back(diffuseTexture);
		}

		if (normalMapTexture != nullptr)
		{
			textures.push_back(normalMapTexture);
		}
	}

	void addTexture(ID3D11ShaderResourceView* texture)
	{
		textures.push_back(texture);
	}

	int indicesCount;
	int indicesOffset;
	bool hasDiffuseTexture;
	bool hasNormalMapTexture;
	string diffuseTextureFile;
	string normalMapTextureFile;
	string specularTextureFile;
	string maskTextureFile;

	XMMATRIX globalTransform;
	ID3D11ShaderResourceView* ambientTexture;
	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11ShaderResourceView* normalMapTexture;
	ID3D11ShaderResourceView* specularTexture;
	ID3D11ShaderResourceView* maskTexture;
	vector<ID3D11ShaderResourceView*> textures;
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

	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT3> tangents;
	vector<XMFLOAT3> binormals;
	vector<UINT> indices;
	vector<XMFLOAT2> uvs;
	UINT verticesCount;
	UINT indicesCount;
	vector<int> indicesOffset;
	vector<XMMATRIX> globalTransforms;
	vector<string> textureFiles;
	int meshesCount;
	bool hasTexture;
	vector<RenderPackage> renderPackages;
	vector<Material*> triangleMaterialIndices;
	vector<MaterialIdOffset> materialIdOffsets;
};