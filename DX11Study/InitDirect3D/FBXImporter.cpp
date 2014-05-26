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
	int verticesCount = mMesh->GetControlPointsCount();
	int indicesCount = mMesh->GetPolygonVertexCount();

	float* vertices = new float[verticesCount * 3];
	float* pV = vertices;
	for (int i = 0; i < verticesCount; i++)
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
	if (verticesCount < 65535)
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
	MeshInfo* meshInfo = new MeshInfo();

	FbxVector4* meshVertices = mMesh->GetControlPoints();
	int verticesCount = mMesh->GetControlPointsCount();
	int indicesCount = mMesh->GetPolygonVertexCount();
	int verticesComponentCount = verticesCount * 3;
	int verticesByteWidth = sizeof(float) * verticesCount * 3;

	// Extract vertices from FBX.
	float* vertices = new float[verticesComponentCount];
	float* pV = vertices;
	for (int i = 0; i < verticesCount; i++)
	{
		pV[i * 3] = static_cast<float>(meshVertices[i][0]);
		pV[i * 3 + 1] = static_cast<float>(meshVertices[i][1]);
		pV[i * 3 + 2] = static_cast<float>(meshVertices[i][2]);
	}

	// Extract indices form FBX.
	UINT* indices = nullptr;
	int* meshIndices = mMesh->GetPolygonVertices();

	// Convert to 16bit index if possible to save memory.
	if (verticesCount < 65535)
	{
		indices = new UINT[indicesCount];

		for (int i = 0; i < indicesCount; i++)
		{
			indices[i] = meshIndices[i];
		}
	}
	else
	{

	}

	float* normals = new float[indicesCount * 3];

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

	meshInfo->vertices.resize(verticesCount * 3);
	meshInfo->indices.resize(indicesCount);
	meshInfo->normals.resize(indicesCount * 3);
	
	memcpy_s(&meshInfo->vertices[0], verticesByteWidth, vertices, verticesByteWidth);
	meshInfo->verticesCount = verticesCount;

	memcpy_s(&meshInfo->indices[0], sizeof(UINT) * indicesCount, indices, sizeof(UINT) * indicesCount);
	meshInfo->indicesCount = indicesCount;

	memcpy_s(&meshInfo->normals[0], sizeof(float) * indicesCount * 3, normals, sizeof(float) * indicesCount * 3);

	for (int i = 0; i < indicesCount; i++)
	{
		DisplayVector(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
	}

	delete[] vertices;
	delete[] indices;

	return meshInfo;
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