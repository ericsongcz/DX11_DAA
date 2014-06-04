#pragma once

#include <fbxsdk.h>
#include "Geometry.h"

struct FBXMeshData
{
	FBXMeshData()
		: mHasTexture(false),
		  mMesh(nullptr),
		  mSurfaceMaterial(nullptr)
	{}

	int mVerticesCount;
	int mIndicesCount;
	int mTrianglesCount;
	vector<XMFLOAT3> mVertices;
	vector<UINT> mIndices;
	vector<XMFLOAT3> mNormals;
	vector<XMFLOAT2> mUVs;
	bool mHasTexture;
	XMMATRIX globalTransform;
	FbxMesh* mMesh;
	FbxSurfaceMaterial* mSurfaceMaterial;
	string textureFileName;
	string textureFilePath;
};

class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();

	void Init();
	void LoadScene(const char* fileName);
	void WalkHierarchy();
	void WalkHierarchy(FbxNode* fbxNode, int depth);
	void ProcessMesh(FbxNodeAttribute* nodeAttribute);
	void SaveData(const char* fileName);

	// 解析网格的基础数据(顶点，法线，材质，纹理等等)。
	MeshData* GetMeshInfo();

	// 读取网格中的法线。
	void ReadNormals(FBXMeshData& fbxMeshData, int contorlPointIndex, int normalIndex); 

	// 读取网格中的纹理坐标。
	void ReadUVs(FBXMeshData& fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// 根据法线类拆分顶点。
	void SplitVertexByNormal(FBXMeshData& fbxMeshData);	

	// 根据UV拆分顶点。
	void SplitVertexByUV(FBXMeshData& fbxMeshData);

	// 计算多边形法线。
	void ComputeNormals(FBXMeshData& fbxMeshData);

	// FbxMatrix到XXMMATRIX的转换。
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount, int* materialIndices);
	void LoadMaterials(FBXMeshData& fbxMeshData);
	void LoadMaterialAttributes(FBXMeshData& fbxMeshData);
	void LoadMaterialTexture(FBXMeshData& fbxMeshData);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	MeshData* mMeshData;
	vector<FBXMeshData> mFBXMeshDatas;
};