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
	MeshInfo* GetMeshInfo();
	void ReadNormals(int contorlPointIndex, int normalIndex, vector<XMFLOAT3>& normals); 
	void ReadUVs(FbxMesh* mesh, int controlPointIndex, int textureUVIndex, int index, int uvLayer, vector<XMFLOAT2>& uvs);
	void SplitVertexByNormal();
	void SplitVertexByUV();
	void SplitVertexByNormalAndUV();
	void ComputeNormals();
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
};