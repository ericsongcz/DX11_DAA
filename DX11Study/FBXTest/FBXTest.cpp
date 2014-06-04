// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

struct Value
{
	int a;
};

class Test
{
public:
	void foo(const Value& value)
	{
		value.a = 10;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}

