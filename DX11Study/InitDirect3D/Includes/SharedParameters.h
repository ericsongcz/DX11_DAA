#include <d3d11.h>
#include <DirectXMath.h>
#include "Shader.h"

using namespace DirectX;

class SharedParameters
{
public:
	static XMMATRIX globalTransform;
	static Shader* shader;
	static ID3D11Device* device;
private:
};