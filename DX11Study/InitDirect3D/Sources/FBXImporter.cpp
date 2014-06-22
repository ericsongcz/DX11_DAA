#include "stdafx.h"
#include <windows.h>
#include "FBXImporter.h"
#include <iostream>
#include <fstream>
#include "D3DUtils.h"
#include "SharedParameters.h"
#include <algorithm>

using namespace std;

FBXImporter::FBXImporter()
: isAllSame(false),
  isByPolygon(false)
{
}

FBXImporter::~FBXImporter()
{
}

void FBXImporter::Init()
{
	mSDKManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(mSDKManager, IOSROOT);
	mSDKManager->SetIOSettings(ios);
	mScene = FbxScene::Create(mSDKManager, "scene");
	FbxAxisSystem::DirectX.ConvertScene(mScene);
}

void FBXImporter::LoadScene(const char* fileName)
{
	FbxImporter* sceneImporter = FbxImporter::Create(mSDKManager, "importer");
	sceneImporter->Initialize(fileName, -1, mSDKManager->GetIOSettings());
	sceneImporter->Import(mScene);
	sceneImporter->Destroy();
}

void FBXImporter::WalkHierarchy()
{
	FbxNode* root = mScene->GetRootNode();
	for (int i = 0; i < root->GetChildCount(); i++)
	{
		WalkHierarchy(root->GetChild(i), 0);
	}
}

void FBXImporter::WalkHierarchy(FbxNode* fbxNode, int depth)
{
	FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();

	if (nodeAttribute == nullptr)
	{
		Log("Name:%s NodeType:None\n", fbxNode->GetName());
	}
	else
	{
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMarker:
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		case FbxNodeAttribute::eMesh:
			Log("Name:%s NodeType:Mesh\n", fbxNode->GetName());
			ProcessMesh(nodeAttribute);
			break;
		case FbxNodeAttribute::eCamera:
			Log("Name:%s NodeType:Camera\n", fbxNode->GetName());
			break;
		case FbxNodeAttribute::eLight:
			Log("Name:%s NodeType:Light\n", fbxNode->GetName());
			break;
		case FbxNodeAttribute::eBoundary:
			break;
		case FbxNodeAttribute::eOpticalMarker:
			break;
		case FbxNodeAttribute::eOpticalReference:
			break;
		case FbxNodeAttribute::eCameraSwitcher:
			break;
		case FbxNodeAttribute::eNull:
			break;
		case FbxNodeAttribute::ePatch:
			break;
		case FbxNodeAttribute::eNurbs:
			break;
		case FbxNodeAttribute::eNurbsSurface:
			break;
		case FbxNodeAttribute::eNurbsCurve:
			break;
		case FbxNodeAttribute::eTrimNurbsSurface:
			break;
		case FbxNodeAttribute::eUnknown:
			break;

		default:
			break;
		}
	}

	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		WalkHierarchy(fbxNode->GetChild(i), depth + 1);
	}
}

void FBXImporter::ProcessMesh(FbxNodeAttribute* nodeAttribute)
{
	FbxMesh* mesh = (FbxMesh*)nodeAttribute;
	
	// �����Ƿ����ǻ���?��������Ƚ���תΪ���ǻ��ġ�
	// ע�⣺һ����ʵӦ���ڽ�ģ���������ʱ����С�
	if (!mesh->IsTriangleMesh())
	{
		FbxGeometryConverter converter(mSDKManager);

		// #1
		// For FBX SDK 2015.1
		nodeAttribute = converter.Triangulate(nodeAttribute, true, false);
		// For FBX SDK 2013.3
		//converter.TriangulateInPlace(nodeAttribute->GetNode());

		mesh = (FbxMesh*)nodeAttribute;
	}

	FBXMeshData fbxMeshData;
	fbxMeshData.mMesh = mesh;
	mFBXMeshDatas.push_back(fbxMeshData);

	Log("TriangleCount:%d\n", mesh->GetPolygonCount());
	Log("VertexCount:%d\n", mesh->GetControlPointsCount());
	Log("IndexCount:%d\n", mesh->GetPolygonVertexCount());
	Log("Layer:%d\n", mesh->GetLayerCount());
	Log("DeformerCount:%d\n", mesh->GetDeformerCount());
	Log("MaterialCount%d\n", mesh->GetNode()->GetMaterialCount());
	Log("\n");
}

MeshData* FBXImporter::GetMeshInfo()
{
	mMeshData = new MeshData();

	int indicesIndexOffset = 0;		// ��¼��ǰmesh������ib�е�����λ�ơ�
	int verticesIndexOffset = 0;	// ��¼��ǰmesh������vb�еĶ���λ�ơ�

	for (int meshIndex = 0; meshIndex < mFBXMeshDatas.size(); meshIndex++)
	{
		FbxMesh* mesh = mFBXMeshDatas[meshIndex].mMesh;
		FBXMeshData& fbxMeshData = mFBXMeshDatas[meshIndex];
		fbxMeshData.mVerticesCount = mesh->GetControlPointsCount();
		fbxMeshData.mIndicesCount = mesh->GetPolygonVertexCount();
		fbxMeshData.mTrianglesCount = mesh->GetPolygonCount();

		// ��ȡ3dsmax�е�ȫ�ֱ任�����Ժ������DX�л�ԭ��
		FbxMatrix gloableTransform = mesh->GetNode()->EvaluateGlobalTransform();

		FbxAMatrix matrixGeo;
		matrixGeo.SetIdentity();

		const FbxVector4 lT = mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);

		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);

		FbxAMatrix matrixL2W;
		matrixL2W.SetIdentity();

		matrixL2W = mesh->GetNode()->EvaluateGlobalTransform();

		matrixL2W *= matrixGeo;

		FbxMatrixToXMMATRIX(fbxMeshData.globalTransform, matrixL2W);

		// ��ȡ���㡣
		ReadVertices(fbxMeshData);
		// ��ȡ������
		ReadIndices(fbxMeshData);

		// �ȶ�ȡ�����Ӧ�Ĳ���������Ϣ���Ա��Ż��Ժ������ȡ��
		// һ���������ֻ��Ӧһ��materialId��Ҳ���ܶ�Ӧ���materialId(3dsmax���Multi/Sub-Object����)��
		// ���ֻ��Ӧһ�����ʣ��򵥵Ķ�ȡ���У������ձ����������Ϊ���Ż���Ⱦ�ϲ�mesh�Ӷ�ӵ�ж���ʡ�
		// �����������������ǻ�õ�materialId��ӵ�����materialId���������б�(�����α���б�)��������vector<MaterialIdOffset>�������С�
		//struct Material
		//{
		//	Material() {}
		//	Material(int id, string diffuse, string normalMap)
		//		: materialId(id),
		//		diffuseTextureFile(diffuse),
		//		normalMapTextureFile(normalMap)
		//	{}
		//
		//	int materialId;
		//	string diffuseTextureFile;
		//	string normalMapTextureFile;
		//};
		// struct MaterialIdOffset
		//{
		//	MaterialIdOffset()
		//		: polygonCount(0)
		//	{}
		//	int polygonCount;
		//	Material material;
		//};
		ConnectMaterialsToMesh(mesh, fbxMeshData.mTrianglesCount);

		// ����ConnectMaterialsToMesh�õ�����Ϣ��ȡ����������Ϣ��ͬ������vector<MaterialIdOffset>������
		LoadMaterials(fbxMeshData);

		int triangleCount = mesh->GetPolygonCount();
		int controlPointIndex = 0;
		int normalIndex = 0;
		fbxMeshData.mUVs.resize(fbxMeshData.mIndicesCount, XMFLOAT2(-1.0f, -1.0f));

		// Extract normals and uvs from FbxMesh.
		for (int i = 0; i < triangleCount; i++)
		{
			int polygonSize = mesh->GetPolygonSize(i);

			for (int j = 0; j < polygonSize; j++)
			{ 
				controlPointIndex = mesh->GetPolygonVertex(i, j);

				ReadNormals(fbxMeshData, controlPointIndex, normalIndex);

				// ���������ǲŶ�ȡuv��tangent�Լ�binormal��
				if (fbxMeshData.mHasDiffuseTexture)
				{
					ReadUVs(fbxMeshData, controlPointIndex, normalIndex, mesh->GetTextureUVIndex(i, j), 0);
					ReadTangents(fbxMeshData, controlPointIndex, normalIndex);
					ReadBinormals(fbxMeshData, controlPointIndex, normalIndex);
				}

				normalIndex++;
			}
		}

		SplitVertexByNormal(fbxMeshData);

		if (fbxMeshData.mHasDiffuseTexture)
		{
			SplitVertexByUV(fbxMeshData);
		}
		else
		{
			fbxMeshData.mUVs.resize(fbxMeshData.mVerticesCount);
		}

		if (fbxMeshData.mHasNormalMapTexture)
		{
			SplitVertexByTangent(fbxMeshData);
			SplitVertexByBinormal(fbxMeshData);
		}
		else
		{
			fbxMeshData.mTangents.resize(fbxMeshData.mVerticesCount);
			fbxMeshData.mBinormals.resize(fbxMeshData.mVerticesCount);
		}

		// ���.fbx����һ�����ϵ�mesh����Ҫ���㵱ǰFBXMeshData��������ȫ�������е�λ�á�
		for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
		{
			fbxMeshData.mIndices[i] = fbxMeshData.mIndices[i] + verticesIndexOffset;
		}

		mMeshData->verticesCount += fbxMeshData.mVerticesCount;
		mMeshData->indicesCount += fbxMeshData.mIndicesCount;
		mMeshData->globalTransforms.push_back(fbxMeshData.globalTransform);
		mMeshData->meshesCount++;

		// ����ʵ������
		// ����֮ǰ����materialIdOffsets���������materialId�������εĶ�Ӧ��ϵ��
		// ����ÿ��RenderPackage��Ⱦ���������������������ʼλ��(ƫ��)��
		if (isByPolygon && fbxMeshData.mHasDiffuseTexture)
		{
			vector<MaterialIdOffset> materialIdOffsets = mMeshData->materialIdOffsets;

			for (int i = 0; i < materialIdOffsets.size(); i++)
			{
				RenderPackage renderPacakge;
				renderPacakge.globalTransform = fbxMeshData.globalTransform;
				renderPacakge.indicesCount = materialIdOffsets[i].polygonCount * 3;
	
				if (i == 0)
				{
					renderPacakge.indicesOffset = indicesIndexOffset;
				}
				else
				{
					renderPacakge.indicesOffset += indicesIndexOffset;
				}

				renderPacakge.diffuseTextureFile = materialIdOffsets[i].material.diffuseTextureFile;
				renderPacakge.normalMapTextureFile = materialIdOffsets[i].material.normalMapTextureFile;

				mMeshData->renderPackages.push_back(renderPacakge);

				indicesIndexOffset += renderPacakge.indicesCount;
			}
		}
		else
		// ��һ���ʵ������
		{
			RenderPackage renderPackage;
			renderPackage.indicesCount = fbxMeshData.mIndicesCount;
			renderPackage.indicesOffset = indicesIndexOffset;
			renderPackage.diffuseTextureFile = fbxMeshData.diffuseTextureFile;
			renderPackage.normalMapTextureFile = fbxMeshData.normalMapTextureFile;
			renderPackage.globalTransform = fbxMeshData.globalTransform;

			mMeshData->renderPackages.push_back(renderPackage);

			indicesIndexOffset += fbxMeshData.mIndices.size();
		}

		verticesIndexOffset += fbxMeshData.mVertices.size();

		// ����ǰmesh������׷�ӵ�ȫ������������
		Merge(mMeshData->vertices, fbxMeshData.mVertices);
		Merge(mMeshData->indices, fbxMeshData.mIndices);
		Merge(mMeshData->normals, fbxMeshData.mNormals);
		Merge(mMeshData->uvs, fbxMeshData.mUVs);
		Merge(mMeshData->tangents, fbxMeshData.mTangents);
		Merge(mMeshData->binormals, fbxMeshData.mBinormals);
		mMeshData->materialIdOffsets.clear();
	}

	mFBXMeshDatas.clear();

	return mMeshData;
}

void FBXImporter::ReadVertices(FBXMeshData& fbxMeshData)
{
	// Extract vertices from FbxMesh.
	FbxVector4* meshVertices = fbxMeshData.mMesh->GetControlPoints();

	for (int i = 0; i < fbxMeshData.mVerticesCount; i++)
	{
		XMFLOAT3 vertex;

		vertex.x = static_cast<float>(meshVertices[i][0]);
		vertex.y = static_cast<float>(meshVertices[i][1]);
		vertex.z = static_cast<float>(meshVertices[i][2]);

		fbxMeshData.mVertices.push_back(vertex);
	}
}

void FBXImporter::ReadIndices(FBXMeshData& fbxMeshData)
{
	// Extract indices form FbxMesh.
	int* meshIndices = fbxMeshData.mMesh->GetPolygonVertices();

	// Convert to 16bit index if possible to save memory.
	for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
	{
		fbxMeshData.mIndices.push_back(meshIndices[i]);
	}
}

void FBXImporter::ReadNormals(FBXMeshData& fbxMeshData, int contorlPointIndex, int normalIndex)
{
	FbxMesh* mesh = fbxMeshData.mMesh;
	vector<XMFLOAT3>& normals = fbxMeshData.mNormals;

	if (mesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementNormal* normalElement = mesh->GetElementNormal(0);
	switch (normalElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (normalElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt(contorlPointIndex);
			XMFLOAT3 normal;
			normal.x = static_cast<float>(fbxNormal[0]);
			normal.y = static_cast<float>(fbxNormal[1]);
			normal.z = static_cast<float>(fbxNormal[2]);
			normals.push_back(normal);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normalElement->GetIndexArray().GetAt(contorlPointIndex);
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt(id);
			XMFLOAT3 normal;
			normal.x = static_cast<float>(fbxNormal[0]);
			normal.y = static_cast<float>(fbxNormal[1]);
			normal.z = static_cast<float>(fbxNormal[2]);
			normals.push_back(normal);
		}
		default:
			break;
		}

		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (normalElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt(normalIndex);
			XMFLOAT3 normal;
			normal.x = static_cast<float>(fbxNormal[0]);
			normal.y = static_cast<float>(fbxNormal[1]);
			normal.z = static_cast<float>(fbxNormal[2]);
			normals.push_back(normal);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normalElement->GetIndexArray().GetAt(normalIndex);
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt(id);
			XMFLOAT3 normal;
			normal.x = static_cast<float>(fbxNormal[0]);
			normal.y = static_cast<float>(fbxNormal[1]);
			normal.z = static_cast<float>(fbxNormal[2]);
			normals.push_back(normal);
		}
		default:
			break;
		}
		break;

	default:
		break;
	}
}

void FBXImporter::ReadTangents(FBXMeshData& fbxMeshData, int controlPointIndex, int tangentIndex)
{
	FbxMesh* mesh = fbxMeshData.mMesh;

	if (mesh->GetElementTangentCount() < 1)
	{
		return;
	}

	FbxGeometryElementTangent* tangentElement = mesh->GetElementTangent(0);

	switch (tangentElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (tangentElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxTangent = tangentElement->GetDirectArray().GetAt(controlPointIndex);
			XMFLOAT3 tangent;
			tangent.x = static_cast<float>(fbxTangent[0]);
			tangent.y = static_cast<float>(fbxTangent[1]);
			tangent.z = static_cast<float>(fbxTangent[2]);
			fbxMeshData.mTangents.push_back(tangent);
		}

		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = tangentElement->GetIndexArray().GetAt(controlPointIndex);
			FbxVector4 fbxTangent = tangentElement->GetDirectArray().GetAt(id);
			XMFLOAT3 tangent;
			tangent.x = static_cast<float>(fbxTangent[0]);
			tangent.y = static_cast<float>(fbxTangent[1]);
			tangent.z = static_cast<float>(fbxTangent[2]);
			fbxMeshData.mTangents.push_back(tangent);
		}

		break;
		default:

		break;
		}
	}

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (tangentElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxTangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
			XMFLOAT3 tangent;
			tangent.x = static_cast<float>(fbxTangent[0]);
			tangent.y = static_cast<float>(fbxTangent[1]);
			tangent.z = static_cast<float>(fbxTangent[2]);
			fbxMeshData.mTangents.push_back(tangent);
		}
			
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = tangentElement->GetIndexArray().GetAt(tangentIndex);
			FbxVector4 fbxTangent = tangentElement->GetDirectArray().GetAt(id);
			XMFLOAT3 tangent;
			tangent.x = static_cast<float>(fbxTangent[0]);
			tangent.y = static_cast<float>(fbxTangent[1]);
			tangent.z = static_cast<float>(fbxTangent[2]);
			fbxMeshData.mTangents.push_back(tangent);
		}
		
		break;
		default:
			break;
		}
	}

		//break;
	default:
		break;
	}
}

void FBXImporter::ReadBinormals(FBXMeshData& fbxMeshData, int controlPointIndex, int tangentIndex)
{
	FbxMesh* mesh = fbxMeshData.mMesh;

	if (mesh->GetElementTangentCount() < 1)
	{
		return;
	}

	FbxGeometryElementBinormal* binormalElement = mesh->GetElementBinormal(0);

	switch (binormalElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (binormalElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxBinormal = binormalElement->GetDirectArray().GetAt(controlPointIndex);
			XMFLOAT3 binormal;
			binormal.x = static_cast<float>(fbxBinormal[0]);
			binormal.y = static_cast<float>(fbxBinormal[1]);
			binormal.z = static_cast<float>(fbxBinormal[2]);
			fbxMeshData.mBinormals.push_back(binormal);
		}

			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = binormalElement->GetIndexArray().GetAt(controlPointIndex);
			FbxVector4 fbxBinormal = binormalElement->GetDirectArray().GetAt(id);
			XMFLOAT3 binormal;
			binormal.x = static_cast<float>(fbxBinormal[0]);
			binormal.y = static_cast<float>(fbxBinormal[1]);
			binormal.z = static_cast<float>(fbxBinormal[2]);
			fbxMeshData.mBinormals.push_back(binormal);
		}

			break;
		default:
			break;
		}
	}

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (binormalElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxBinormal = binormalElement->GetDirectArray().GetAt(tangentIndex);
			XMFLOAT3 binormal;
			binormal.x = static_cast<float>(fbxBinormal[0]);
			binormal.y = static_cast<float>(fbxBinormal[1]);
			binormal.z = static_cast<float>(fbxBinormal[2]);
			fbxMeshData.mBinormals.push_back(binormal);
		}

			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = binormalElement->GetIndexArray().GetAt(tangentIndex);
			FbxVector4 fbxBinormal = binormalElement->GetDirectArray().GetAt(id);
			XMFLOAT3 binormal;
			binormal.x = static_cast<float>(fbxBinormal[0]);
			binormal.y = static_cast<float>(fbxBinormal[1]);
			binormal.z = static_cast<float>(fbxBinormal[2]);
			fbxMeshData.mBinormals.push_back(binormal);
		}

			break;
		default:
			break;
		}
	}

		break;
	default:
		break;
	}
}

void FBXImporter::ReadUVs(FBXMeshData& fbxMeshData, int controlPointIndex, int index, int textureUVIndex, int uvLayer)
{
	FbxMesh* mesh = fbxMeshData.mMesh;
	vector<XMFLOAT2>& uvs = fbxMeshData.mUVs;

	if (uvLayer >= 2 || mesh->GetElementUVCount() <= uvLayer)
	{
		return;
	}

	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			XMFLOAT2 uv;
			FbxVector2 fbxUV = vertexUV->GetDirectArray().GetAt(controlPointIndex);
			uv.x = static_cast<float>(fbxUV[0]);
			uv.y = static_cast<float>(fbxUV[1]);
			uvs.push_back(uv);
		}
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			FbxVector2 fbxUV = vertexUV->GetDirectArray().GetAt(id);
			XMFLOAT2 uv;
			uv.x = static_cast<float>(fbxUV[0]);
			uv.y = static_cast<float>(fbxUV[1]);
			uvs.push_back(uv);
		}

		break;
		default:
			break;
		}

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
#if USE_RIGHT_HAND
			uvs[index].x = static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex)[0]);
#else
			uvs[index].x = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex)[0]);
#endif
			uvs[index].y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex)[1]);
			break;
		default:
			break;
		}
		break;
	}
}

void FBXImporter::SplitVertexByNormal(FBXMeshData& fbxMeshData)
{
	vector<XMFLOAT3> normals;
	normals.resize(fbxMeshData.mVerticesCount, XMFLOAT3(0.0f, 0.0f, 0.0f));

	vector<UINT>& indicesBuffer = fbxMeshData.mIndices;
	vector<XMFLOAT3>& verticesBuffer = fbxMeshData.mVertices;

	// �������������ݷ��������ֶ��㣬ʹ��ÿ���������Ψһ�ķ���(�����������)��
	// ����˼·�����Ƚ�һ���Ͷ���������ͬ�ߴ�ķ������飬Ȼ��������˳�������������顣
	// �ڱ����Ĺ����У����ǻ���������λ����ͬ�����Ƿ��߲�ͬ����������ʱ�����Ǿ�
	// ���䶥�����飬��������㸴��һ��׷�ӵ���������β����Ȼ����¶�Ӧ��������ͬʱ
	// ���ǽ�����¶����Ӧ�ķ��ߴ��뷨�����顣
	for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
	{
		if (XMFLOAT3Equal(normals[indicesBuffer[i]], XMFLOAT3(0.0f, 0.0f, 0.0f)))
		{              
			normals[indicesBuffer[i]] = fbxMeshData.mNormals[i];
		}
		else if (!XMFLOAT3Equal(normals[indicesBuffer[i]], fbxMeshData.mNormals[i]))
		{
			// ���󶥵����飬���µĶ���׷�ӵ�ĩβ��
			verticesBuffer.resize(fbxMeshData.mVerticesCount + 1);
			verticesBuffer[fbxMeshData.mVerticesCount] = verticesBuffer[indicesBuffer[i]];

			// Ȼ�������������������
			indicesBuffer[i] = fbxMeshData.mVerticesCount;
			fbxMeshData.mVerticesCount++;

			// ���淨�ߡ�
			normals.push_back(fbxMeshData.mNormals[i]);
		}
	}

	fbxMeshData.mNormals = normals;
}

void FBXImporter::SplitVertexByTangent(FBXMeshData& fbxMeshData)
{
	vector<XMFLOAT3> tangents;
	tangents.resize(fbxMeshData.mVerticesCount, XMFLOAT3(0.0f, 0.0f, 0.0f));

	vector<UINT>& indicesBuffer = fbxMeshData.mIndices;
	vector<XMFLOAT3>& verticesBuffer = fbxMeshData.mVertices;

	// �������������ݷ��������ֶ��㣬ʹ��ÿ���������Ψһ������(�����������)��
	// ����˼·�����Ƚ�һ���Ͷ���������ͬ�ߴ���������飬Ȼ��������˳�������������顣
	// �ڱ����Ĺ����У����ǻ���������λ����ͬ���������߲�ͬ����������ʱ�����Ǿ�
	// ���䶥�����飬��������㸴��һ��׷�ӵ���������β����Ȼ����¶�Ӧ��������ͬʱ
	// ���ǽ�����¶����Ӧ�����ߴ����������顣
	for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
	{
		if (XMFLOAT3Equal(tangents[indicesBuffer[i]], XMFLOAT3(0.0f, 0.0f, 0.0f)))
		{
			tangents[indicesBuffer[i]] = fbxMeshData.mTangents[i];
		}
		else if (!XMFLOAT3Equal(tangents[indicesBuffer[i]], fbxMeshData.mTangents[i]))
		{
			// ���󶥵����飬���µĶ���׷�ӵ�ĩβ��
			verticesBuffer.resize(fbxMeshData.mVerticesCount + 1);
			verticesBuffer[fbxMeshData.mVerticesCount] = verticesBuffer[indicesBuffer[i]];

			// Ȼ�������������������
			indicesBuffer[i] = fbxMeshData.mVerticesCount;
			fbxMeshData.mVerticesCount++;

			// ���淨�ߡ�
			tangents.push_back(fbxMeshData.mTangents[i]);
		}
	}

	fbxMeshData.mTangents = tangents;
}

void FBXImporter::SplitVertexByBinormal(FBXMeshData& fbxMeshData)
{
	vector<XMFLOAT3> binormals;
	binormals.resize(fbxMeshData.mVerticesCount, XMFLOAT3(0.0f, 0.0f, 0.0f));

	vector<UINT>& indicesBuffer = fbxMeshData.mIndices;
	vector<XMFLOAT3>& verticesBuffer = fbxMeshData.mVertices;

	// �������������ݸ����������ֶ��㣬ʹ��ÿ���������Ψһ�ĸ�����(�����������)��
	// ����˼·�����Ƚ�һ���Ͷ���������ͬ�ߴ�ĸ��������飬Ȼ��������˳�������������顣
	// �ڱ����Ĺ����У����ǻ���������λ����ͬ�����Ǹ����߲�ͬ����������ʱ�����Ǿ�
	// ���䶥�����飬��������㸴��һ��׷�ӵ���������β����Ȼ����¶�Ӧ��������ͬʱ
	// ���ǽ�����¶����Ӧ�ĸ����ߴ��븱�������顣
	for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
	{
		if (XMFLOAT3Equal(binormals[indicesBuffer[i]], XMFLOAT3(0.0f, 0.0f, 0.0f)))
		{
			binormals[indicesBuffer[i]] = fbxMeshData.mBinormals[i];
		}
		else if (!XMFLOAT3Equal(binormals[indicesBuffer[i]], fbxMeshData.mBinormals[i]))
		{
			// ���󶥵����飬���µĶ���׷�ӵ�ĩβ��
			verticesBuffer.resize(fbxMeshData.mVerticesCount + 1);
			verticesBuffer[fbxMeshData.mVerticesCount] = verticesBuffer[indicesBuffer[i]];

			// Ȼ�������������������
			indicesBuffer[i] = fbxMeshData.mVerticesCount;
			fbxMeshData.mVerticesCount++;

			// ���淨�ߡ�
			binormals.push_back(fbxMeshData.mBinormals[i]);
		}
	}

	fbxMeshData.mBinormals = binormals;
}

void FBXImporter::SplitVertexByUV(FBXMeshData& fbxMeshData)
{
	vector<XMFLOAT2> uvs;
	uvs.resize(fbxMeshData.mVerticesCount, XMFLOAT2(-1.0f, -1.0f));

	vector<UINT>& indicesBuffer = fbxMeshData.mIndices;
	vector<XMFLOAT3>& verticesBuffer = fbxMeshData.mVertices;
	
	// ��SplitVertexByNormal��һ����ִ�С�
	// ԭ����UV���ܻ�ʹ�����һ�����࣬��������������һ���Ѿ�����ȷ����
	// ��������������UV�����ֶ��㣬ʹ��ÿ�����㶼����Ψһ��UV(���������ܻ��һ������)��
	// ˼·�͸��ݷ��߻��ֶ������ơ�
	for (int i = 0; i < fbxMeshData.mIndicesCount; i++)
	{
		if (XMFLOAT2Equal(uvs[indicesBuffer[i]], XMFLOAT2(-1.0f, -1.0f)))
		{
			uvs[indicesBuffer[i]] = fbxMeshData.mUVs[i];
		} 
		else if (!XMFLOAT2Equal(uvs[indicesBuffer[i]], fbxMeshData.mUVs[i]))
		{			
			// ���󶥵����飬���µĶ���׷�ӵ�ĩβ��
			verticesBuffer.resize(fbxMeshData.mVerticesCount + 1);
			verticesBuffer[fbxMeshData.mVerticesCount] = verticesBuffer[indicesBuffer[i]];

			// ��Ϊ�����������ˣ��������ǻ���Ҫ���������飬˼·�ͻ��ֶ���һ����
			fbxMeshData.mNormals.resize(fbxMeshData.mVerticesCount + 1);
			fbxMeshData.mNormals[fbxMeshData.mVerticesCount] = fbxMeshData.mNormals[indicesBuffer[i]];

			// Ȼ�������������������
			indicesBuffer[i] = fbxMeshData.mVerticesCount;

			// ���¶�������
			fbxMeshData.mVerticesCount++;

			// ����գ֡�
			uvs.push_back(fbxMeshData.mUVs[i]);
		}
	}

	fbxMeshData.mUVs = uvs;
}

void FBXImporter::ComputeNormals(FBXMeshData& fbxMeshData)
{
	for (int i = 0; i < fbxMeshData.mTrianglesCount; i++)
	{
		// Indices of the ith triangle.
		UINT i0 = mMeshData->indices[i * 3];
		UINT i1 = mMeshData->indices[i * 3 + 1];
		UINT i2 = mMeshData->indices[i * 3 + 2];

		// Vertices of ith triangle.
		XMFLOAT3 pos0 = mMeshData->vertices[i0];
		XMFLOAT3 pos1 = mMeshData->vertices[i1];
		XMFLOAT3 pos2 = mMeshData->vertices[i2];

		XMFLOAT3 e0;
		XMFLOAT3 e1;
		XMFLOAT3Sub(e0, pos1, pos0);
		XMFLOAT3Sub(e1, pos2, pos0);

		XMFLOAT3 normal;
		XMFLOAT3Cross(normal, e0, e1);

		XMFLOAT3Add(mMeshData->normals[i0], mMeshData->normals[i0], normal);
		XMFLOAT3Add(mMeshData->normals[i1], mMeshData->normals[i1], normal);
		XMFLOAT3Add(mMeshData->normals[i2], mMeshData->normals[i2], normal);
	}

	for (int i = 0; i <fbxMeshData.mVerticesCount; i++)
	{
		XMFLOAT3Normalize(mMeshData->normals[i], mMeshData->normals[i]);
	}
}

void FBXImporter::FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in)
{
	out = XMMatrixSet(in.Get(0, 0), in.Get(0, 1), in.Get(0, 2), in.Get(0, 3),
					  in.Get(1, 0), in.Get(1, 1), in.Get(1, 2), in.Get(1, 3),
					  in.Get(2, 0), in.Get(2, 1), in.Get(2, 2), in.Get(2, 3),
					  in.Get(3, 0), in.Get(3, 1), in.Get(3, 2), in.Get(3, 3));
}

void FBXImporter::ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount)
{
	// Get the material index list of current mesh.
	FbxLayerElementArrayTemplate<int>* materialIndices;
	FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;

	if (mesh->GetElementMaterial() != nullptr)
	{
		// Get the material indices.
		materialIndices = &mesh->GetElementMaterial()->GetIndexArray();
		materialMappingMode = mesh->GetElementMaterial()->GetMappingMode();

		switch (materialMappingMode)
		{
		// ��mesh����ʵ������������Ҫ������������������������ID��
		case FbxLayerElement::eByPolygon:

			isByPolygon = true;
			isAllSame = false;

			if (materialIndices->GetCount() == triangleCount)
			{
				int materialId = materialIndices->GetAt(0);
				int polygonCount = 0;
				MaterialIdOffset offset;
				vector<MaterialIdOffset>& offsets = mMeshData->materialIdOffsets;

				for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
				{
					int materialIndex = materialIndices->GetAt(triangleIndex);

					// �Ƚϵ�ǰmaterialId���ϴ�ѭ����materialId��
					// �����ͬ�Ļ�˵�������ڴ���ӵ��ͬһ��materialId�������μ��ϣ�
					// ����ֻ��Ҫ���������μ���������materialId��
					if (materialId == materialIndex)
					{
						offset.polygonCount++;
						offset.material.materialId = materialIndex;

						// ����Ѿ��������һ�������Σ��������ƫ����Ϣ��
						if (triangleIndex == triangleCount - 1)
						{
							offsets.push_back(offset);
						}
					}
					else
					// �����ǰmaterialId���ϴ�ѭ���Ĳ�ͬ��˵����ʼ���µ������μ���
					// ��ô���Ǿͽ��ɵļ��ϱ��浽��������ʼ�����¼��ϡ�
					{

						offsets.push_back(offset);
						offset.material.materialId = materialIndex;
						offset.polygonCount = 0;
						offset.polygonCount++;
					}

					materialId = materialIndex;
				}
			}

			break;

		case FbxLayerElement::eAllSame:
		{
			isAllSame = true;
			isByPolygon = false;

			int materialIndex = materialIndices->GetAt(0);

			MaterialIdOffset offset;
			vector<MaterialIdOffset>& offsets = mMeshData->materialIdOffsets;

			offset.material.materialId = materialIndex;
			offset.polygonCount = triangleCount;

			offsets.push_back(offset);
		}

			break;
		default:
			break;
		}
	}
}

void FBXImporter::LoadMaterials(FBXMeshData& fbxMeshData)
{
	FbxNode* node = nullptr;
	FbxMesh* mesh = fbxMeshData.mMesh;
	int materialCount = 0;
	int polygonCount = mesh->GetPolygonCount();

	if ((mesh != nullptr) && (mesh->GetNode() != nullptr))
	{
		node = mesh->GetNode();
		materialCount = node->GetMaterialCount();
	}

	bool isAllSame = true;
	for (int i = 0; i < mesh->GetElementMaterialCount(); i++)
	{
		FbxGeometryElementMaterial* materialElement = mesh->GetElementMaterial(i);
		if (materialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			isAllSame = false;
			break;
		}
	}

	//For eAllSame mapping type, just out the material and texture mapping info once
	if (isAllSame)
	{
		for (int i = 0; i < mesh->GetElementMaterialCount(); i++)
		{
			FbxGeometryElementMaterial* materialElement = mesh->GetElementMaterial(i);
			if (materialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(materialElement->GetIndexArray().GetAt(0));
				fbxMeshData.mSurfaceMaterial = material;
				int materialId = materialElement->GetIndexArray().GetAt(0);
				if (materialId >= 0)
				{
					fbxMeshData.diffuseTextureFile.clear();
					fbxMeshData.normalMapTextureFile.clear();
					LoadMaterialTexture(fbxMeshData, FbxSurfaceMaterial::sDiffuse);
					LoadMaterialTexture(fbxMeshData, FbxSurfaceMaterial::sBump);

					vector<string>& textureFiles = mMeshData->textureFiles;
					auto iter = find(textureFiles.begin(), textureFiles.end(), fbxMeshData.diffuseTextureFile);

					if (iter == textureFiles.end())
					{
						textureFiles.push_back(fbxMeshData.diffuseTextureFile);
					}

					if (fbxMeshData.normalMapTextureFile.size() > 0)
					{
						iter = find(textureFiles.begin(), textureFiles.end(), fbxMeshData.normalMapTextureFile);
						if (iter == textureFiles.end())
						{
							textureFiles.push_back(fbxMeshData.normalMapTextureFile);
						}
					}
				}
			}
		}
	}
	//For eByPolygon mapping type, just out the material and texture mapping info once
	else
	{
		int materialId = 0;
		int polygonId = 0;
		polygonCount = 0;
		vector<string>& textureFiles = mMeshData->textureFiles;
		vector<MaterialIdOffset>& materialIdOffsets = mMeshData->materialIdOffsets;

		for (int i = 0; i < materialIdOffsets.size(); i++)
		{
			FbxGeometryElementMaterial* materialElement = mesh->GetElementMaterial(0);
			FbxSurfaceMaterial* material = NULL;
			materialId = mMeshData->materialIdOffsets[i].material.materialId;

			material = mesh->GetNode()->GetMaterial(materialElement->GetIndexArray().GetAt(polygonId));
			polygonCount = materialIdOffsets[i].polygonCount;

			fbxMeshData.mSurfaceMaterial = material;

			fbxMeshData.diffuseTextureFile.clear();
			fbxMeshData.normalMapTextureFile.clear();
			LoadMaterialTexture(fbxMeshData, FbxSurfaceMaterial::sDiffuse);
			LoadMaterialTexture(fbxMeshData, FbxSurfaceMaterial::sBump);

			materialIdOffsets[i].material.diffuseTextureFile = fbxMeshData.diffuseTextureFile;

			auto iter = find(textureFiles.begin(), textureFiles.end(), fbxMeshData.diffuseTextureFile);

			if (iter == textureFiles.end())
			{
				textureFiles.push_back(fbxMeshData.diffuseTextureFile);
			}

			if (fbxMeshData.normalMapTextureFile.size() > 0)
			{
				iter = find(textureFiles.begin(), textureFiles.end(), fbxMeshData.normalMapTextureFile);
				if (iter == textureFiles.end())
				{
					textureFiles.push_back(fbxMeshData.normalMapTextureFile);
				}

				materialIdOffsets[i].material.normalMapTextureFile = fbxMeshData.normalMapTextureFile;
			}

			polygonId += polygonCount;
		}
	}
}

void FBXImporter::LoadMaterialAttributes(FBXMeshData& fbxMeshData)
{
	// Get the name of material.
	FbxSurfaceMaterial* surfaceMaterial = fbxMeshData.mSurfaceMaterial;
	const char* materialName = surfaceMaterial->GetName();

	// Phong material
	if (surfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		// Ambient color.
		FbxDouble3 ambient = ((FbxSurfacePhong*)surfaceMaterial)->Ambient;

		// Diffuse color.
		FbxDouble3 diffuse = ((FbxSurfacePhong*)surfaceMaterial)->Diffuse;

		// Specular color.
		FbxDouble3 specular = ((FbxSurfacePhong*)surfaceMaterial)->Specular;

		// Emissive color.
		FbxDouble3 emissive = ((FbxSurfacePhong*)surfaceMaterial)->Emissive;

		// Opacity.
		FbxDouble opacity = ((FbxSurfacePhong*)surfaceMaterial)->TransparencyFactor;

		// Shininess.
		FbxDouble shininess = ((FbxSurfacePhong*)surfaceMaterial)->Shininess;

		// Reflectivety.
		FbxDouble reflectiveity = ((FbxSurfacePhong*)surfaceMaterial)->ReflectionFactor;
	}

	// Lambert material.
	if (surfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Ambient color.
		FbxDouble3 ambient = ((FbxSurfaceLambert*)surfaceMaterial)->Ambient;

		// Diffuse color.
		FbxDouble3 diffuse = ((FbxSurfaceLambert*)surfaceMaterial)->Diffuse;

		// Emissive color.
		FbxDouble3 emissive = ((FbxSurfaceLambert*)surfaceMaterial)->Emissive;

		// Opacity.
		FbxDouble opacity = ((FbxSurfaceLambert*)surfaceMaterial)->TransparencyFactor;
	}
}

void FBXImporter::LoadMaterialTexture(FBXMeshData& fbxMeshData, const char* textureType)
{
	FbxProperty property;

	// #NoteReference to DisplayMesh.cxx in FBX SDK samples.Note#
	property = fbxMeshData.mSurfaceMaterial->FindProperty(textureType);

	if (property.IsValid())
	{
		int textureCount = property.GetSrcObjectCount<FbxTexture>();

		if (textureCount > 0)
		{
			if (strcmp(textureType, FbxSurfaceMaterial::sDiffuse) == 0)
			{
				fbxMeshData.mHasDiffuseTexture = true;
			}
			else if (strcmp(textureType, FbxSurfaceMaterial::sBump) == 0)
			{
				fbxMeshData.mHasNormalMapTexture = true;
			}
		}

		for (int i = 0; i < textureCount; i++)
		{
			FbxTexture* texture = FbxCast<FbxTexture>(property.GetSrcObject<FbxTexture>(i));

			if (texture != nullptr)
			{
				FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

				if (strcmp(textureType, FbxSurfaceMaterial::sDiffuse) == 0)
				{
					fbxMeshData.diffuseTextureFile = fileTexture->GetFileName();
				}
				else if (strcmp(textureType, FbxSurfaceMaterial::sBump) == 0)
				{
					fbxMeshData.normalMapTextureFile = fileTexture->GetFileName();
				}
			}
		}
	}
}
