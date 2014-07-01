#include <DirectXMath.h>
#include "Shader.h"
#include "Camera.h"
#include "Direct3DRenderer.h"
#include "ShaderUntils.h"
#include <vector>

using std::vector;
using namespace DirectX;

class SharedParameters
{
public:
	static XMMATRIX globalTransform;
	static Shader* shader;
	static Camera* camera;
	static vector<RenderPackage> renderPackages;
	static XMMATRIX rotate;
	static bool showTexture;
	static Direct3DRenderer* render;
	static XMMATRIX worldMatrix;
	static ESamplerType samplerType;
private:
};