#include "stdafx.h"
#include "SharedParameters.h"

XMMATRIX SharedParameters::globalTransform = XMMatrixIdentity();
Shader* SharedParameters::shader = nullptr;
Camera* SharedParameters::camera = nullptr;
ID3D11Device* SharedParameters::device = nullptr;
ID3D11DeviceContext* SharedParameters::deviceContext = nullptr;
vector<RenderPackage> SharedParameters::renderPackages = vector<RenderPackage>();
XMMATRIX SharedParameters::rotate = XMMatrixIdentity();
bool SharedParameters::showTexture = false;
Direct3DRenderer* SharedParameters::render = nullptr;
XMMATRIX SharedParameters::worldMatrix = XMMatrixIdentity();