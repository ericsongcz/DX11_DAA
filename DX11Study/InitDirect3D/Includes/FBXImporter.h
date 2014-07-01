#pragma once
//****************************************************************************//
//FBXImporter.h                                                               //
//****************************************************************************//

#include <fbxsdk.h>
#include "Geometry.h"

struct FBXMeshData
{
	FBXMeshData()
		: mVerticesCount(0),
		mIndicesCount(0),
		mTrianglesCount(0),
		mTangentCount(0),
		mBinormalCount(0),
		mMesh(nullptr),
		mSurfaceMaterial(nullptr),
		mMaterial(new Material())
	{}

	~FBXMeshData()
	{
		clear();
	}

	string getDiffuseTextureFile() const
	{
		return mMaterial->getDiffuseTextureFile();
	}

	string getNormalMapTextureFile() const
	{
		return mMaterial->getNormalMapTextureFile();
	}

	void clear()
	{
		SafeDestroy(mSurfaceMaterial);
		SafeDestroy(mMesh);
	}

	int mVerticesCount;
	int mIndicesCount;
	int mTrianglesCount;
	int mTangentCount;
	int mBinormalCount;
	vector<XMFLOAT3> mVertices;
	vector<UINT> mIndices;
	vector<XMFLOAT3> mNormals;
	vector<XMFLOAT3> mTangents;
	vector<XMFLOAT3> mBinormals;
	vector<XMFLOAT2> mUVs;
	bool mHasDiffuseTexture;
	bool mHasNormalMapTexture;
	XMFLOAT4X4 globalTransform;
	FbxMesh* mMesh;
	FbxSurfaceMaterial* mSurfaceMaterial;
	Material* mMaterial;
	string mSpecularTextureFile;
	string mMaskTextureFile;
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
	void ReadVertices(FBXMeshData* fbxMeshData);

	// ��ȡ����������Ϣ��
	void ReadIndices(FBXMeshData* fbxMeshData);

	// ��ȡ����ķ�����Ϣ��
	void ReadNormals(FBXMeshData* fbxMeshData, int contorlPointIndex, int normalIndex); 

	// ��ȡ�����������Ϣ��
	void ReadTangents(FBXMeshData* fbxMeshData, int contorlPointIndex, int tangentIndex);

	// ��ȡ����ĸ�������Ϣ��
	void ReadBinormals(FBXMeshData* fbxMeshData, int controlPointIndex, int binormalIndex);

	// ��ȡ������������ꡣ
	void ReadUVs(FBXMeshData* fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// ���ݷ��߲�ֶ��㡣
	void SplitVertexByNormal(FBXMeshData* fbxMeshData);	

	// �������߲�ֶ���(?)��
	void SplitVertexByTangent(FBXMeshData* fbxMeshData);

	// �������߲�ֶ���(?)��
	void SplitVertexByBinormal(FBXMeshData* fbxMeshData);

	// ����UV��ֶ��㡣
	void SplitVertexByUV(FBXMeshData* fbxMeshData);

	// �������η��ߡ�
	void ComputeNormals(FBXMeshData* fbxMeshData);

	// FbxMatrix��XXMMATRIX��ת����
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);

	// ��ȡ���������б�(�������ζ�Ӧ)�Լ���ӵ��ͬһ��materialId�������η��顣
	// �����Ϳ��Լ���LoadMaterialTexture�����ĵ���(������Ҫ����triangleCount��ѭ������)��
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount);

	// ��ȡFbxSurfaceMaterialʵ����
	void LoadMaterials(FBXMeshData* fbxMeshData);

	void LoadMaterialAttributes(FBXMeshData* fbxMeshData);

	// ��ȡ��ӦFbxSurfaceTexture�ж�Ӧ������
	void LoadMaterialTexture(FBXMeshData* fbxMeshData, const char* textureType);

	void clear();
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	MeshData* mMeshData;
	vector<FBXMeshData*> mFBXMeshDatas;
	bool isByPolygon;
	bool isAllSame;
};