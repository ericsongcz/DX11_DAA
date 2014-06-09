// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <DirectXMath.h>
#include "../InitDirect3D/Includes/GameTimer.h"

using namespace DirectX;
using namespace std;

struct Object
{
	int value;
};

void foo(Object* objects, int numObjects)
{
	for (int i = 0; i < numObjects; i++)
	{
		cout << objects[i].value << endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Object obj1;
	obj1.value = 10;

	Object obj2;
	obj2.value = 20;

	vector<Object> objects;
	objects.push_back(obj1);
	objects.push_back(obj2);

	foo(&objects[0], objects.size());

	return 0;
}

