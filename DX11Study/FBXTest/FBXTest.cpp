// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FBXImporter.h"
#include <ctime>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	FBXImporter* importer = new FBXImporter();
	importer->Init();
	importer->LoadScene("Cube.fbx");
	importer->WalkHierarchy();
	importer->SaveData("Cube.bin");

	srand((int)time(nullptr));

	for (int i = 0; i < 10; i++)
	{
		cout << rand() / (float)(RAND_MAX + 1) << endl;
	}

	cout << RAND_MAX / (float)(RAND_MAX + 1) << endl;

	return 0;
}

