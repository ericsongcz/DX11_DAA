#include "stdafx.h"
#include "SharedParameters.h"

XMMATRIX SharedParameters::globalTransform = XMMatrixIdentity();
Shader* SharedParameters::shader = nullptr;
ID3D11Device* SharedParameters::device = nullptr;