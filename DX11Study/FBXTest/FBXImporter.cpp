#include "stdafx.h"
#include <windows.h>
#include "FBXImporter.h"
#include <iostream>
#include <fstream>

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

	short* indices = nullptr;
	int* meshIndices = mMesh->GetPolygonVertices();

	// Convert to 16bit index if possible to save memory.
	if (verticesCount < 65535)
	{
		indices = new short[indicesCount];
		short* currentIndices = indices;

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