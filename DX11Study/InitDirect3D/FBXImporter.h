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
	void SplitVertexByNormal();
	void ComputeNormals();
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	FbxMesh* mMesh;
	MeshInfo* mMeshInfo;
	int mVerticesCount;
	int mIndicesCount;
	int mTrianglesCount;
	vector<XMFLOAT3> mNormals;
};