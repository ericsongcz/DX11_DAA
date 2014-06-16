#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "Camera.h"
#include "Direct3DRenderer.h"
#include <vector>

using std::vector;
using namespace DirectX;

class SharedParameters
{
public:
	static XMMATRIX globalTransform;
	static Shader* shader;
	static Camera* camera;
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
	static vector<RenderPackage> renderPackages;
	static XMMATRIX rotate;
	static bool showTexture;
	static Direct3DRenderer* render;
private:
};