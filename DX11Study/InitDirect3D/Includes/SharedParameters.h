#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"
#include "Camera.h"

using namespace DirectX;

class SharedParameters
{
public:
	static XMMATRIX globalTransform;
	static Shader* shader;
	static Camera* camera;
	static ID3D11Device* device;
private:
};