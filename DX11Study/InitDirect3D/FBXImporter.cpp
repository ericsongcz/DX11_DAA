#include "stdafx.h"
#include <windows.h>
#include "FBXImporter.h"
#include <iostream>
#include <fstream>
#include "D3DUtils.h"

using namespace std;

FBXImporter::FBXImporter()
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
		cout << "Name:" << fbxNode->GetName() << " NodeType:None" << endl;
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
			cout << "Name:" << fbxNode->GetName() << " NodeType:Mesh" << endl;
			ProcessMesh(nodeAttribute);
			break;
		case FbxNodeAttribute::eCamera:
			cout << "Name:" << fbxNode->GetName() << " NodeType:Camera" << endl;
			break;
		case FbxNodeAttribute::eLight:
			cout << "Name:" << fbxNode->GetName() << " NodeType:Light" << endl;
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
	mMesh = (FbxMesh*)nodeAttribute;

	if (!mMesh->IsTriangleMesh())
	{
		FbxGeometryConverter converter(mSDKManager);

		// #1
		nodeAttribute = converter.Triangulate(nodeAttribute, true, false);
		mMesh = (FbxMesh*)nodeAttribute;
	}

	cout << "TriangleCount:" << mMesh->GetPolygonCount()
		 << " VertexCount:" << mMesh->GetControlPointsCount()
		 << " IndexCount:" << mMesh->GetPolygonVertexCount()
		 << " Layer:" << mMesh->GetLayerCount()
		 << " DeformerCount:" << mMesh->GetDeformerCount()
		 << " MaterialCount:" << nodeAttribute->GetNode()->GetMaterialCount()
		 << endl;
}

void FBXImporter::SaveData(const char* fileName)
{
	FbxVector4* meshVertices = mMesh->GetControlPoints();
	mVerticesCount = mMesh->GetControlPointsCount();
	int indicesCount = mMesh->GetPolygonVertexCount();

	float* vertices = new float[mVerticesCount * 3];
	float* pV = vertices;
	for (int i = 0; i < mVerticesCount; i++)
	{
		*pV = static_cast<float>(meshVertices[i][0]);
		pV++;
		*pV = static_cast<float>(meshVertices[i][1]);
		pV++;
		*pV = static_cast<float>(meshVertices[i][2]);
		pV++;
	}

	ofstream out(fileName, ios_base::out | ios_base::binary);

	// Write geometryInfo: vertex and index count.
	int geometryInfo[2] = { 48, 49 };
	out.write(reinterpret_cast<const char*>(geometryInfo), sizeof (int) * 2);

	out.close();

	ifstream in("Cube.bin", ios_base::binary);
	char buffer[2];
	in.read(buffer, 2);

	printf("buffer:%s", buffer);

	in.close();

	int* indices = nullptr;
	int* meshIndices = mMesh->GetPolygonVertices();

	// Convert to 16bit index if possible to save memory.
	if (mVerticesCount < 65535)
	{
		indices = new int[indicesCount];
		int* currentIndices = indices;

		for (int i = 0; i < indicesCount; i++)
		{
			*currentIndices = meshIndices[i];
		}
	}
	else
	{

	}

	delete[] vertices;
	delete[] indices;
}

MeshInfo* FBXImporter::GetMeshInfo()
{
	mMeshInfo = new MeshInfo();

	FbxVector4* meshVertices = mMesh->GetControlPoints();
	mVerticesCount = mMesh->GetControlPointsCount();
	mIndicesCount = mMesh->GetPolygonVertexCount();
	int verticesComponentCount = mVerticesCount * 3;
	int verticesByteWidth = sizeof(float) * mVerticesCount * 3;

	// Extract vertices from FBX.
	float* vertices = new float[verticesComponentCount];
	float* pV = vertices;
	for (int i = 0; i < mVerticesCount; i++)
	{
		pV[i * 3] = static_cast<float>(meshVertices[i][0]);
		pV[i * 3 + 1] = static_cast<float>(meshVertices[i][1]);
		pV[i * 3 + 2] = static_cast<float>(meshVertices[i][2]);
	}

	// Extract indices form FBX.
	UINT* indices = nullptr;
	int* meshIndices = mMesh->GetPolygonVertices();

	// Convert to 16bit index if possible to save memory.
	if (mVerticesCount < 65535)
	{
		indices = new UINT[mIndicesCount];

		for (int i = 0; i < mIndicesCount; i++)
		{
			indices[i] = meshIndices[i];
		}
	}
	else
	{

	}

	float* normals = new float[mIndicesCount * 3];

	int triangleCount = mMesh->GetPolygonCount();
	int controlPointIndex = 0;
	int normalIndex = 0;

	for (int i = 0; i < triangleCount; i++)
	{
		int polygonSize = mMesh->GetPolygonSize(i);

		for (int j = 0; j < polygonSize; j++)
		{
			controlPointIndex = mMesh->GetPolygonVertex(i, j);

			ReadNormals(controlPointIndex, normalIndex, normals);

			normalIndex++;
		}
	}

	mMeshInfo->vertices.resize(mVerticesCount * 3);
	mMeshInfo->indices.resize(mIndicesCount);
	mMeshInfo->normals.resize(mIndicesCount * 3);
	
	memcpy_s(&mMeshInfo->vertices[0], verticesByteWidth, vertices, verticesByteWidth);
	mMeshInfo->verticesCount = mVerticesCount;

	memcpy_s(&mMeshInfo->indices[0], sizeof(UINT) * mIndicesCount, indices, sizeof(UINT) * mIndicesCount);
	mMeshInfo->indicesCount = mIndicesCount;

	memcpy_s(&mMeshInfo->normals[0], sizeof(float) * mIndicesCount * 3, normals, sizeof(float) * mIndicesCount * 3);

	for (int i = 0; i < mIndicesCount; i++)
	{
		DisplayVector(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
	}

	SplitVertexByNormal();

	delete[] vertices;
	delete[] indices;

	return mMeshInfo;
}

void FBXImporter::ReadNormals(int contorlPointIndex, int normalIndex, float* normals)
{
	if (mMesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementNormal* normal = mMesh->GetElementNormal(0);
	switch (normal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(contorlPointIndex);
			normals[normalIndex * 3] = static_cast<float>(fbxNormal[0]);
			normals[normalIndex * 3 + 1] = static_cast<float>(fbxNormal[1]);
			normals[normalIndex * 3 + 2] = static_cast<float>(fbxNormal[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normal->GetIndexArray().GetAt(contorlPointIndex);
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(id);
			normals[normalIndex * 3] = static_cast<float>(fbxNormal[0]);
			normals[normalIndex * 3 + 1] = static_cast<float>(fbxNormal[1]);
			normals[normalIndex * 3 + 2] = static_cast<float>(fbxNormal[2]);
		}
		default:
			break;
		}

		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (normal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(normalIndex);
			normals[normalIndex * 3] = static_cast<float>(fbxNormal[0]);
			normals[normalIndex * 3 + 1] = static_cast<float>(fbxNormal[1]);
			normals[normalIndex * 3 + 2] = static_cast<float>(fbxNormal[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normal->GetIndexArray().GetAt(normalIndex);
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(id);
			normals[normalIndex * 3] = static_cast<float>(fbxNormal[0]);
			normals[normalIndex * 3 + 1] = static_cast<float>(fbxNormal[1]);
			normals[normalIndex * 3 + 2] = static_cast<float>(fbxNormal[2]);
		}
		default:
			break;
		}
		break;

	default:
		break;
	}
}

void FBXImporter::SplitVertexByNormal()
{
	int verticesCount = mVerticesCount;

	vector<float> normals;
	normals.resize(verticesCount * 3, 0.0f);

	vector<UINT> indicesBuffer = mMeshInfo->indices;

	for (int i = 0; i < mIndicesCount; i++)
	{
		if ((normals[indicesBuffer[i] * 3] == 0.0f) && (normals[indicesBuffer[i] * 3 + 1] == 0.0f) && (normals[indicesBuffer[i] * 3 + 2] == 0.0f))
		{
			normals[indicesBuffer[i] * 3] = mMeshInfo->normals[i * 3];
			normals[indicesBuffer[i] * 3 + 1] = mMeshInfo->normals[i * 3 + 1];
			normals[indicesBuffer[i] * 3 + 2] = mMeshInfo->normals[i * 3 + 2];
		}
		else if ((normals[indicesBuffer[i] * 3] != mMeshInfo->normals[i * 3]) &&
				(normals[indicesBuffer[i] * 3 + 1] != mMeshInfo->normals[i * 3 + 1]) &&
				(normals[indicesBuffer[i] * 3 + 2] != mMeshInfo->normals[i * 3 + 2]))
		{
			mMeshInfo->vertices.resize(verticesCount + 1);
			mMeshInfo->vertices[verticesCount * 3] = mMeshInfo->vertices[mMeshInfo->indices[i] * 3];
			mMeshInfo->vertices[verticesCount * 3 + 1] = mMeshInfo->vertices[mMeshInfo->indices[i] * 3 + 1];
			mMeshInfo->vertices[verticesCount * 3 + 2] = mMeshInfo->vertices[mMeshInfo->indices[i] * 3 + 2];
			mMeshInfo->indices[i] = verticesCount;
			verticesCount++;
		}
	}
}