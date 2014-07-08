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

int _tmain(int argc, _TCHAR* argv[])
{
	string str = "256,256";
	char* token = ",";

	size_t pos = str.find(token, 0);

	system("pause");

	return 0;
}

