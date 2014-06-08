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
	vector<int> vect(100000);

	GameTimer timer;
	timer.Reset();

	for (size_t i = 0; i < vect.size(); i++)
	{
		cout << vect[i] << endl;
		timer.Tick();
	}

	cout << timer.DeltaTime() << endl;

	return 0;
}

