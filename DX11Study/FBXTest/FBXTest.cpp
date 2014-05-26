// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

void ReadIndices(int* indices, int index)
{
	indices[index * 3] = index;
	indices[index * 3 + 1] = index + 1;
	indices[index * 3 + 2] = index + 2;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int* indices = new int[9];

	for (int i = 0; i < 3; i++)
	{
		ReadIndices(indices, i);
	}

	for (int i = 0; i < 9; i++)
	{
		cout << indices[i] << endl;
	}

	return 0;
}

