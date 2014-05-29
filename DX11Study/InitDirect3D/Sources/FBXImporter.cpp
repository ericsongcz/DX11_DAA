#include "stdafx.h"
#include <windows.h>
#include "FBXImporter.h"
#include <iostream>
#include <fstream>
#include "D3DUtils.h"
#include "SharedParameters.h"

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
		// For FBX SDK 2015.1
		nodeAttribute = converter.Triangulate(nodeAttribute, true, false);
		// For FBX SDK 2013.3
		//converter.TriangulateInPlace(nodeAttribute->GetNode());

		mMesh = (FbxMesh*)nodeAttribute;
	}

	cout << "TriangleCount:" << mMesh->GetPolygonCount() << endl;
	cout << "VertexCount:" << mMesh->GetControlPointsCount() << endl;
	cout << "IndexCount:" << mMesh->GetPolygonVertexCount() << endl;
	cout << "Layer:" << mMesh->GetLayerCount() << endl;
	cout << "DeformerCount:" << mMesh->GetDeformerCount() << endl;
	cout << "MaterialCount:" << nodeAttribute->GetNode()->GetMaterialCount() << endl;
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

	FbxMatrixToXMMATRIX(SharedParameters::globalTransform, gloableTransform);

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
	mUVs.resize(mIndicesCount);

	int triangleCount = mMesh->GetPolygonCount();
	int controlPointIndex = 0;
	int normalIndex = 0;
	vector<XMFLOAT2> uvs(mIndicesCount);

	for (int i = 0; i < triangleCount; i++)
	{
		int polygonSize = mMesh->GetPolygonSize(i);

		for (int j = 0; j < polygonSize; j++)
		{
			controlPointIndex = mMesh->GetPolygonVertex(i, j);

			ReadNormals(controlPointIndex, normalIndex, mNormals);

			ReadUVs(mMesh, controlPointIndex, normalIndex, mMesh->GetTextureUVIndex(i, j), 0, mUVs);

			normalIndex++;
		}
	}

	mMeshInfo->vertices = verticesPositions;
	mMeshInfo->indices = indices;
	mMeshInfo->uvs = uvs;
	mMeshInfo->normals.resize(mVerticesCount);

	//ComputeNormals();
	SplitVertexByUV();
	SplitVertexByNormal();
	mMeshInfo->normals = mNormals;
	mMeshInfo->uvs = mUVs;

	mMeshInfo->verticesCount = mMeshInfo->vertices.size();
	mMeshInfo->indicesCount = mIndicesCount;

	int* triangleMaterialIndices = new int[mTrianglesCount];

	ConnectMaterialsToMesh(mMesh, mTrianglesCount, triangleMaterialIndices);
	LoadMaterials(mMesh);

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

void FBXImporter::ReadUVs(FbxMesh* mesh, int controlPointIndex, int textureUVIndex, int index, int uvLayer, vector<XMFLOAT2>& uvs)
{
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
			uvs[index].x = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex)[0]);
			uvs[index].y = static_cast<float>(vertexUV->GetDirectArray().GetAt(controlPointIndex)[1]);

			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexUV->GetIndexArray().GetAt(controlPointIndex);
			uvs[index].x = static_cast<float>(vertexUV->GetDirectArray().GetAt(id)[0]);
			uvs[index].y = static_cast<float>(vertexUV->GetDirectArray().GetAt(id)[1]);
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
			uvs[index].x = static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex)[0]);
			uvs[index].y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(textureUVIndex)[1]);
			break;
		default:
			break;
		}
		break;
	}
}

void FBXImporter::SplitVertexByNormal()
{
	int verticesCount = mMeshInfo->vertices.size();

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

	for (int i = 0; i < mNormals.size(); i++)
	{
		XMFLOAT3Negative(mNormals[i], mNormals[i]);
	}
}

void FBXImporter::SplitVertexByUV()
{
	int verticesCount = mVerticesCount;
	vector<XMFLOAT2> uvs;
	uvs.resize(verticesCount, XMFLOAT2(0.0f, 0.0f));

	vector<UINT> indicesBuffer = mMeshInfo->indices;

	for (int i = 0; i < mIndicesCount; i++)
	{
		if (XMFLOAT2Equal(uvs[indicesBuffer[i]], XMFLOAT2(0.0f, 0.0f)))
		{
			uvs[indicesBuffer[i]] = mUVs[i];
		}
		else if (!XMFLOAT2Equal(uvs[indicesBuffer[i]], mUVs[i]))
		{
			mMeshInfo->vertices.resize(verticesCount + 1);
			mMeshInfo->vertices[verticesCount] = mMeshInfo->vertices[mMeshInfo->indices[i]];
			mMeshInfo->indices[i] = verticesCount;
			verticesCount++;
			uvs.push_back(mUVs[i]);
		}
	}

	mUVs = uvs;
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

void FBXImporter::ConnectMaterialsToMesh(FbxMesh* mesh, int triangleCount, int* triangleMaterialIndices)
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
		case FbxLayerElement::eByPolygon:
			if (materialIndices->GetCount() == triangleCount)
			{
				for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
				{
					int materialIndex = materialIndices->GetAt(triangleIndex);
					triangleMaterialIndices[triangleIndex] = materialIndex;
				}
			}

			break;

		case FbxLayerElement::eAllSame:
		{
			int materialIndex = materialIndices->GetAt(0);

			for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				triangleMaterialIndices[triangleIndex] = materialIndex;
			}
		}

			break;
		default:
			break;
		}
	}
}

void FBXImporter::LoadMaterials(FbxMesh* mesh)
{
	int materialCount = 0;
	int polygonCount = 0;
	FbxNode* node = nullptr;

	if ((mesh != nullptr) && (mesh->GetNode() != nullptr))
	{
		node = mesh->GetNode();
		materialCount = node->GetMaterialCount();
	}

	bool isAllSame = true;
	for (int i = 0; i < mMesh->GetElementMaterialCount(); i++)
	{
		FbxGeometryElementMaterial* materialElement = mMesh->GetElementMaterial(i);
		if (materialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			isAllSame = false;
			break;
		}
	}
	//For eAllSame mapping type, just out the material and texture mapping info once
	if (isAllSame)
	{
		for (int i = 0; i < mMesh->GetElementMaterialCount(); i++)
		{
			FbxGeometryElementMaterial* materialElement = mMesh->GetElementMaterial(i);
			if (materialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				FbxSurfaceMaterial* material = mMesh->GetNode()->GetMaterial(materialElement->GetIndexArray().GetAt(0));
				int materialId = materialElement->GetIndexArray().GetAt(0);
				if (materialId >= 0)
				{
					LoadMaterialAttributes(material);
				}
			}
		}
	}
	//For eByPolygon mapping type, just out the material and texture mapping info once
	else
	{
		for (int i = 0; i < polygonCount; i++)
		{
			for (int j = 0; j < mMesh->GetElementMaterialCount(); j++)
			{
				FbxGeometryElementMaterial* materialElement = mMesh->GetElementMaterial(j);
				FbxSurfaceMaterial* material = NULL;
				int materialId = -1;
				material = mMesh->GetNode()->GetMaterial(materialElement->GetIndexArray().GetAt(i));
				materialId = materialElement->GetIndexArray().GetAt(i);

				if (materialId >= 0)
				{
				}
			}
		}
	}
}

void FBXImporter::LoadMaterialAttributes(FbxSurfaceMaterial* surfaceMaterial)
{
	// Get the name of material.
	const char* materialName = surfaceMaterial->GetName();

	Log("Material name:%s\n", materialName);

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

	LoadMaterialTexture(surfaceMaterial);
}

void FBXImporter::LoadMaterialTexture(FbxSurfaceMaterial* surfaceMaterial)
{
	int textureLayerIndex;
	FbxProperty property;
	int textureId;

	//for (textureLayerIndex = 0; textureLayerIndex < FbxLayerElement::sTypeTextureCount; textureLayerIndex++)
	//{
	//	property = surfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureLayerIndex]);

	//	if (property.IsValid())
	//	{
	//		int textureCount = property.GetSrcObjectCount<FbxTexture>();

	//		for (int i = 0; i < textureCount; i++)
	//		{
	//			FbxTexture* texture = FbxCast<FbxTexture>(property.GetSrcObject<FbxTexture>(i));

	//			if (texture != nullptr)
	//			{
	//				texture->
	//				Log("Texture name:%s\n", texture->GetName());
	//			}
	//		}
	//	}
	//}

	// #NoteReference to DisplayMesh.cxx in FBX SDK samples.Note#
	property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	if (property.IsValid())
	{
		int textureCount = property.GetSrcObjectCount<FbxTexture>();

		for (int i = 0; i < textureCount; i++)
		{
			FbxTexture* texture = FbxCast<FbxTexture>(property.GetSrcObject<FbxTexture>(i));

			if (texture != nullptr)
			{
				Log("Texture name:%s\n", texture->GetName());

				FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

				Log("Texture file name:%s\n", fileTexture->GetFileName());

				mMeshInfo->textureFilePath = fileTexture->GetFileName();
			}
		}
	}
}