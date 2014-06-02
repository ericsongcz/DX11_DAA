// FBXTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 10.0f));
	XMVECTOR lookAt = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));

	XMMATRIX viewMatrixLH = XMMatrixLookAtLH(eye, lookAt, up);
	XMMATRIX viewMatrixRH = XMMatrixLookAtRH(eye, lookAt, up);

	return 0;
}

