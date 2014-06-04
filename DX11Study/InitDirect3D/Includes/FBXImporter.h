#pragma once

#include <fbxsdk.h>
#include "Geometry.h"

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
	MeshInfo* GetMeshInfo();

	// 读取网格中的法线。
	void ReadNormals(int contorlPointIndex, int normalIndex, vector<XMFLOAT3>& normals); 

	// 读取网格中的纹理坐标。
	void ReadUVs(FbxMesh* mesh, int controlPointIndex, int textureUVIndex, int index, int uvLayer, vector<XMFLOAT2>& uvs);

	// 根据法线类拆分顶点。
	void SplitVertexByNormal();	

	// 根据UV拆分顶点。
	void SplitVertexByUV();

	// 计算多边形法线。
	void ComputeNormals();

	// FbxMatrix到XXMMATRIX的转换。
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount, int* materialIndices);
	void LoadMaterials(FbxMesh* mesh);
	void LoadMaterialAttributes(FbxSurfaceMaterial* surfaceMaterial);
	void LoadMaterialTexture(FbxSurfaceMaterial* surfaceMaterial);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	FbxMesh* mMesh;
	MeshInfo* mMeshInfo;
	int mVerticesCount;
	int mIndicesCount;
	int mTrianglesCount;
	vector<XMFLOAT3> mNormals;
	vector<XMFLOAT2> mUVs;
	bool mHasTexture;
};