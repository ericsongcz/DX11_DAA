#include "stdafx.h"
#include "SharedD3DDevice.h"

ID3D11Device* SharedD3DDevice::device = nullptr;
ID3D11DeviceContext* SharedD3DDevice::deviceContext = nullptr;