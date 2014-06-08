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
	void ReadVertices(FBXMeshData& fbxMeshData);

	// 读取网格索引信息。
	void ReadIndices(FBXMeshData& fbxMeshData);

	// 读取网格的法线信息。
	void ReadNormals(FBXMeshData& fbxMeshData, int contorlPointIndex, int normalIndex); 

	// 读取网格的切线信息。
	void ReadTangents(FBXMeshData& fbxMeshData, int contorlPointIndex, int tangentIndex);

	// 读取网格的纹理坐标。
	void ReadUVs(FBXMeshData& fbxMeshData, int controlPointIndex, int textureUVIndex, int index, int uvLayer);

	// 根据法线拆分顶点。
	void SplitVertexByNormal(FBXMeshData& fbxMeshData);	

	// 根据UV拆分顶点。
	void SplitVertexByUV(FBXMeshData& fbxMeshData);

	// 计算多边形法线。
	void ComputeNormals(FBXMeshData& fbxMeshData);

	// FbxMatrix到XXMMATRIX的转换。
	void FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in);

	// 获取材质索引列表(和三角形对应)以及分组拥有同一个materialId的三角形。
	void ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount);

	// 获取FbxSurfaceMaterial实例。
	void LoadMaterials(FBXMeshData& fbxMeshData);

	void LoadMaterialAttributes(FBXMeshData& fbxMeshData);

	// 获取对应FbxSurfaceTexture中对应的纹理。
	string LoadMaterialTexture(FBXMeshData& fbxMeshData);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	MeshData* mMeshData;
	vector<FBXMeshData> mFBXMeshDatas;
	bool isByPolygon;
	bool isAllSame;
};