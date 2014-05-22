#include <d3dx11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

class Geometry
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void renderBuffer();
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11InputLayout* mInputLayout;
	DWORD mVerticesCount;
	DWORD mIndicesCount;
};