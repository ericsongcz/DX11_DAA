#pragma once

#include <fbxsdk.h>

class FBXImporter
{
public:
	FBXImporter();
	~FBXImporter();

	void Init();
	void LoadScene(const char* fileName);
	void WalkHierarchy();
	void WalkHierarchy(FbxNode* fbxNode, int depth);
	void ProcessMesh(FbxNodeAttribute* nodeAttribute);
	void SaveData(const char* fileName);
private:
	FbxManager* mSDKManager;
	FbxScene* mScene;
	FbxMesh* mMesh;
};