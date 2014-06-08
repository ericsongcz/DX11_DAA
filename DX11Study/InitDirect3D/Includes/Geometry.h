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
	XMFLOAT2 texcoord;
};

struct Material
{
	Material(int id, string file)
	: materialId(id),
	  diffuseTextureFile(file)
	{}

	int materialId;
	string diffuseTextureFile;
};

struct MaterialIdOffset
{
	MaterialIdOffset()
	: materialId(0),
	  polygonCount(0)
	{}
	int materialId;
	int polygonCount;
	string diffuseTextureFile;
};

struct RenderPackage
{
	RenderPackage()
	: indicesCount(0),
	  indicesOffset(0),
	  ambientTexture(nullptr),
	  diffuseTexture(nullptr),
	  normalMapTexture(nullptr),
	  specularTexture(nullptr),
	  maskTexture(nullptr)
	{}

	int indicesCount;
	int indicesOffset;
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
	void renderBuffer(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	MeshData* GetMeshData() const;
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