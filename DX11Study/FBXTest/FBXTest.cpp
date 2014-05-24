// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FBXImporter.h"

int _tmain(int argc, _TCHAR* argv[])
{
	FBXImporter* importer = new FBXImporter();
	importer->Init();
	importer->LoadScene("Cube.fbx");
	importer->WalkHierarchy();
	importer->SaveData("Cube.bin");

	return 0;
}

