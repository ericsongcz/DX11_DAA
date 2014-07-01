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

	// 初始化FbxSDK。
	void Init();

	// 加载.fbx文件。
	void LoadScene(const char* fileName);

	// 编译场景节点。
	void WalkHierarchy();
	void WalkHierarchy(FbxNode* fbxNode, int depth);

	// 循环处理FbxMesh集合。
	void ProcessMesh(FbxNodeAttribute* nodeAttribute);

	// 解析网格的基础数据(顶点，法线，材质，纹理等等)。
	// 按照材质划分将几何体划分为不同的RenderPackge。
	// 每个RenderPackge为一个渲染批次(也就是一个Drawindexed调用)。
	MeshData* GetMeshInfo();

	// 读取网格顶点信息。
	void ReadVertices(FBXMeshData* fbxMeshData);

	// 读取网格索引信息。
	void ReadIndices(FBXMeshData* fbxMeshData);

	// 读取网格的法线信息。
	void ReadNormals(FBXMeshData* fbxMeshData, int contorlPointIndex, int normalIndex); 

	// 读取网格的切线信息。
	void ReadTangents(FBXMeshData* fbxMeshData, int contorlPointIndex, int tangentIndex);

	// 读取网格的副法线信息。
	void ReadBinormals(FBXMeshData* fbxMeshData, int controlPointIndex, int binormalIndex);

	// 读取网格的纹理坐标。
	void ReadUVs(FBXMeshData* fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// 根据法线拆分顶点。
	void SplitVertexByNormal(FBXMeshData* fbxMeshData);	

	// 根据切线拆分顶点(?)。
	void SplitVertexByTangent(FBXMeshData* fbxMeshData);

	// 根据切线拆分顶点(?)。
	void SplitVertexByBinormal(FBXMeshData* fbxMeshData);

	// 根据UV拆分顶点。
	void SplitVertexByUV(FBXMeshData* fbxMeshData);

	// 计算多边形法线。
	void ComputeNormals(FBXMeshData* fbxMeshData);

	// FbxMatrix到XXMMATRIX的转换。
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);

	// 获取材质索引列表(和三角形对应)以及将拥有同一个materialId的三角形分组。
	// 这样就可以减少LoadMaterialTexture函数的调用(否则需要基于triangleCount来循环调用)。
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount);

	// 获取FbxSurfaceMaterial实例。
	void LoadMaterials(FBXMeshData* fbxMeshData);

	void LoadMaterialAttributes(FBXMeshData* fbxMeshData);

	// 获取对应FbxSurfaceTexture中对应的纹理。
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