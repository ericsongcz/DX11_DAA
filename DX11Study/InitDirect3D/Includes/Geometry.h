#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

using namespace DirectX;
using std::vector;
using std::string;

struct Vertex
{
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
	  rotate(XMMatrixIdentity()),
	  showTexture(false)
	{}

	bool hasDiffuseTexture;
	bool hasNormalMapTexture;
	XMFLOAT4 ambientColor;
	float ambientIntensity;
	XMFLOAT4 diffuseColor;
	float diffuseIntensity;
	XMMATRIX rotate;
	bool showTexture;
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

class Geometry
{
public:
	Geometry();
	~Geometry();

	void FillMeshData(MeshData* meshData);
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void setupBuffers(ID3D11DeviceContext* deviceContext);
	MeshData* GetMeshData() const;
	void clear();
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11InputLayout* mInputLayout;
	int mVerticesCount;
	int mIndicesCount;
	Vertex* mVertices;
	UINT* mIndices;
	MeshData* mMeshdata;
};