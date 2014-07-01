#include "stdafx.h"
#include "SharedParameters.h"

XMMATRIX SharedParameters::globalTransform = XMMatrixIdentity();
Shader* SharedParameters::shader = nullptr;
Camera* SharedParameters::camera = nullptr;
vector<RenderPackage> SharedParameters::renderPackages = vector<RenderPackage>();
XMMATRIX SharedParameters::rotate = XMMatrixIdentity();
bool SharedParameters::showTexture = false;
Direct3DRenderer* SharedParameters::render = nullptr;
XMMATRIX SharedParameters::worldMatrix = XMMatrixIdentity();
ESamplerType SharedParameters::samplerType = ST_LINEAR;