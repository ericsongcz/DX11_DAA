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

	// ��������Ļ�������(���㣬���ߣ����ʣ�����ȵ�)��
	MeshData* GetMeshInfo();

	// ��ȡ�����еķ��ߡ�
	void ReadNormals(FBXMeshData& fbxMeshData, int contorlPointIndex, int normalIndex); 

	// ��ȡ�����е��������ꡣ
	void ReadUVs(FBXMeshData& fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// ���ݷ������ֶ��㡣
	void SplitVertexByNormal(FBXMeshData& fbxMeshData);	

	// ����UV��ֶ��㡣
	void SplitVertexByUV(FBXMeshData& fbxMeshData);

	// �������η��ߡ�
	void ComputeNormals(FBXMeshData& fbxMeshData);

	// FbxMatrix��XXMMATRIX��ת����
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