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
	mTrianglesCount = mMesh->GetPolygonCount();

	FbxMatrix gloableTransform = mMesh->GetNode()->EvaluateGlobalTransform();

	FbxMatrixToXMMATRIX(mMeshInfo->worldTransform, gloableTransform);

	int verticesComponentCount = mVerticesCount * 3;
	int verticesByteWidth = sizeof(float) * mVerticesCount * 3;

	// Extract vertices from FBX.
	vector<XMFLOAT3> verticesPositions;
	verticesPositions.resize(mVerticesCount);

	for (int i = 0; i < mVerticesCount; i++)
	{
		verticesPositions[i].x = static_cast<float>(meshVertices[i][0]);
		verticesPositions[i].y = static_cast<float>(meshVertices[i][1]);
		verticesPositions[i].z = static_cast<float>(meshVertices[i][2]);
	}

	// Extract indices form FBX.
	int* meshIndices = mMesh->GetPolygonVertices();
	vector<UINT> indices;
	indices.resize(mIndicesCount);

	// Convert to 16bit index if possible to save memory.
	if (mVerticesCount < 65535)
	{
		for (int i = 0; i < mIndicesCount; i++)
		{
			indices[i] = meshIndices[i];
		}
	}
	else
	{

	}

	mNormals.resize(mIndicesCount);

	int triangleCount = mMesh->GetPolygonCount();
	int controlPointIndex = 0;
	int normalIndex = 0;

	for (int i = 0; i < triangleCount; i++)
	{
		int polygonSize = mMesh->GetPolygonSize(i);

		for (int j = 0; j < polygonSize; j++)
		{
			controlPointIndex = mMesh->GetPolygonVertex(i, j);

			ReadNormals(controlPointIndex, normalIndex, mNormals);

			normalIndex++;
		}
	}

	mMeshInfo->vertices = verticesPositions;
	mMeshInfo->indices = indices;
	mMeshInfo->normals.resize(mVerticesCount);

	//ComputeNormals();
	SplitVertexByNormal();
	mMeshInfo->normals = mNormals;

	mMeshInfo->verticesCount = mMeshInfo->vertices.size();
	mMeshInfo->indicesCount = mIndicesCount;

	return mMeshInfo;
}

void FBXImporter::ReadNormals(int contorlPointIndex, int normalIndex, vector<XMFLOAT3>& normals)
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
			normals[normalIndex].x = static_cast<float>(fbxNormal[0]);
			normals[normalIndex].y = static_cast<float>(fbxNormal[1]);
			normals[normalIndex].z = static_cast<float>(fbxNormal[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normal->GetIndexArray().GetAt(contorlPointIndex);
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(id);
			normals[normalIndex].x = static_cast<float>(fbxNormal[0]);
			normals[normalIndex].y = static_cast<float>(fbxNormal[1]);
			normals[normalIndex].z = static_cast<float>(fbxNormal[2]);
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
			normals[normalIndex].x = static_cast<float>(fbxNormal[0]);
			normals[normalIndex].y = static_cast<float>(fbxNormal[1]);
			normals[normalIndex].z = static_cast<float>(fbxNormal[2]);
		}
			break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = normal->GetIndexArray().GetAt(normalIndex);
			FbxVector4 fbxNormal = normal->GetDirectArray().GetAt(id);
			normals[normalIndex].x = static_cast<float>(fbxNormal[0]);
			normals[normalIndex].y = static_cast<float>(fbxNormal[1]);
			normals[normalIndex].z = static_cast<float>(fbxNormal[2]);
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

	vector<XMFLOAT3> normals;
	normals.resize(verticesCount, XMFLOAT3(0.0f, 0.0f, 0.0f));

	vector<UINT> indicesBuffer = mMeshInfo->indices;

	for (int i = 0; i < mIndicesCount; i++)
	{
		if (XMFLOAT3Equal(normals[indicesBuffer[i]], XMFLOAT3(0.0f, 0.0f, 0.0f)))
		{
			normals[indicesBuffer[i]] = mNormals[i];
		}
		else if (!XMFLOAT3Equal(normals[indicesBuffer[i]], mNormals[i]))
		{
			mMeshInfo->vertices.resize(verticesCount + 1);
			mMeshInfo->vertices[verticesCount] = mMeshInfo->vertices[mMeshInfo->indices[i]];
			mMeshInfo->indices[i] = verticesCount;
			verticesCount++;
			normals.push_back(mNormals[i]);
		}
	}

	mNormals = normals;
}

void FBXImporter::ComputeNormals()
{
	for (int i = 0; i < mTrianglesCount; i++)
	{
		// Indices of the ith triangle.
		UINT i0 = mMeshInfo->indices[i * 3];
		UINT i1 = mMeshInfo->indices[i * 3 + 1];
		UINT i2 = mMeshInfo->indices[i * 3 + 2];

		// Vertices of ith triangle.
		XMFLOAT3 pos0 = mMeshInfo->vertices[i0];
		XMFLOAT3 pos1 = mMeshInfo->vertices[i1];
		XMFLOAT3 pos2 = mMeshInfo->vertices[i2];

		XMFLOAT3 e0;
		XMFLOAT3 e1;
		XMFLOAT3Sub(e0, pos1, pos0);
		XMFLOAT3Sub(e1, pos2, pos0);

		XMFLOAT3 normal;
		XMFLOAT3Cross(normal, e0, e1);

		XMFLOAT3Add(mMeshInfo->normals[i0], mMeshInfo->normals[i0], normal);
		XMFLOAT3Add(mMeshInfo->normals[i1], mMeshInfo->normals[i1], normal);
		XMFLOAT3Add(mMeshInfo->normals[i2], mMeshInfo->normals[i2], normal);
	}

	for (int i = 0; i < mVerticesCount; i++)
	{
		XMFLOAT3Normalize(mMeshInfo->normals[i], mMeshInfo->normals[i]);
	}
}

void FBXImporter::FbxMatrixToXMMATRIX(XMMATRIX& out, const FbxMatrix& in)
{
	out = XMMatrixSet(in.Get(0, 0), in.Get(0, 1), in.Get(0, 2), in.Get(0, 3),
					  in.Get(1, 0), in.Get(1, 1), in.Get(1, 2), in.Get(1, 3),
					  in.Get(2, 0), in.Get(2, 1), in.Get(2, 2), in.Get(2, 3),
					  in.Get(3, 0), in.Get(3, 1), in.Get(3, 2), in.Get(3, 3));
}