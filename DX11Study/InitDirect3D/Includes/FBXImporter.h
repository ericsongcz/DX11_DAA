#pragma once
//****************************************************************************//
//FBXImporter.h                                                               //
//****************************************************************************//

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
	string diffuseTextureFile;
	string normalMapTextureFile;
	string specularTextureFile;
	string maskTextureFile;
};

class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();

	// ��ʼ��FbxSDK��
	void Init();

	// ����.fbx�ļ���
	void LoadScene(const char* fileName);

	// ���볡���ڵ㡣
	void WalkHierarchy();
	void WalkHierarchy(FbxNode* fbxNode, int depth);

	// ѭ������FbxMesh���ϡ�
	void ProcessMesh(FbxNodeAttribute* nodeAttribute);

	// ��������Ļ�������(���㣬���ߣ����ʣ�����ȵ�)��
	// ���ղ��ʻ��ֽ������廮��Ϊ��ͬ��RenderPackge��
	// ÿ��RenderPackgeΪһ����Ⱦ����(Ҳ����һ��Drawindexed����)��
	MeshData* GetMeshInfo();

	// ��ȡ���񶥵���Ϣ��
	void ReadVertices(FBXMeshData& fbxMeshData);

	// ��ȡ����������Ϣ��
	void ReadIndices(FBXMeshData& fbxMeshData);

	// ��ȡ����ķ�����Ϣ��
	void ReadNormals(FBXMeshData& fbxMeshData, int contorlPointIndex, int normalIndex); 

	// ��ȡ�����������Ϣ��
	void ReadTangents(FBXMeshData& fbxMeshData, int contorlPointIndex, int tangentIndex);

	// ��ȡ������������ꡣ
	void ReadUVs(FBXMeshData& fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// ���ݷ��߲�ֶ��㡣
	void SplitVertexByNormal(FBXMeshData& fbxMeshData);	

	// ����UV��ֶ��㡣
	void SplitVertexByUV(FBXMeshData& fbxMeshData);

	// �������η��ߡ�
	void ComputeNormals(FBXMeshData& fbxMeshData);

	// FbxMatrix��XXMMATRIX��ת����
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);

	// ��ȡ���������б�(�������ζ�Ӧ)�Լ�����ӵ��ͬһ��materialId�������Ρ�
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount);

	// ��ȡFbxSurfaceMaterialʵ����
	void LoadMaterials(FBXMeshData& fbxMeshData);

	void LoadMaterialAttributes(FBXMeshData& fbxMeshData);

	// ��ȡ��ӦFbxSurfaceTexture�ж�Ӧ������
	string LoadMaterialTexture(FBXMeshData& fbxMeshData);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	MeshData* mMeshData;
	vector<FBXMeshData> mFBXMeshDatas;
	bool isByPolygon;
	bool isAllSame;
};