#include "stdafx.h"
#include "Shader.h"
#include "D3DUtils.h"
#include <fstream>
#include "SharedParameters.h"

Shader::Shader()
: mMatrixBuffer(nullptr),
  mInputLayout(nullptr),
  mVertexShader(nullptr),
  mPixelShader(nullptr),
  mSamplerState(nullptr),
  mDeviceContext(nullptr),
  mShaderResourceView(nullptr)
{
	SharedParameters::shader = this;
}

Shader::~Shader()
{

}

bool Shader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	// 从磁盘加载编译好的Vertex Shader源文件。
	std::ifstream shaderFile;
	size_t vsShaderSize = 0;
	size_t psShaderSize = 0;
	char* vsShaderData;
	char* psShaderData;

	shaderFile.open(vsFileName, std::ifstream::in | std::ifstream::binary);

	if (shaderFile.good())
	{
		shaderFile.seekg(0, std::ios::end);
		vsShaderSize = size_t(shaderFile.tellg());
		vsShaderData = new char[vsShaderSize];
		shaderFile.seekg(0, std::ios::beg);
		shaderFile.read(&vsShaderData[0], vsShaderSize);
	}
	else
	{
		return false;
	}

	HR(mDevice->CreateVertexShader(vsShaderData, vsShaderSize, nullptr, &mVertexShader));

	// 读第二个文件之前先要close。
	shaderFile.close();

	// 从磁盘加载编译好的Pixel Shader源文件。
	shaderFile.open(psFileName, std::ifstream::in | std::ifstream::binary);

	if (shaderFile.good())
	{
		shaderFile.seekg(0, std::ios::end);
		psShaderSize = size_t(shaderFile.tellg());
		psShaderData = new char[psShaderSize];
		shaderFile.seekg(0, std::ios::beg);
		shaderFile.read(&psShaderData[0], psShaderSize);
	}
	else
	{
		return false;
	}

	HR(mDevice->CreatePixelShader(psShaderData, psShaderSize, nullptr, &mPixelShader));

	delete[] psShaderData;

	// 设置数据布局，以便在Shader中使用。
	// 定义要和顶点结构一致。
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[5];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[3], sizeof(D3D11_INPUT_ELEMENT_DESC));
	poloygonLayout[0].SemanticName = "POSITION";	// VS中的输入参数。
	poloygonLayout[0].SemanticIndex = 0;
	poloygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[0].InputSlot = 0;
	poloygonLayout[0].AlignedByteOffset = 0;
	poloygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[0].InstanceDataStepRate = 0;

	poloygonLayout[1].SemanticName = "COLOR";
	poloygonLayout[1].SemanticIndex = 0;
	poloygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	poloygonLayout[1].InputSlot = 0;
	poloygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[1].InstanceDataStepRate = 0;

	poloygonLayout[2].SemanticName = "NORMAL";
	poloygonLayout[2].SemanticIndex = 0;
	poloygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[2].InputSlot = 0;
	poloygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[2].InstanceDataStepRate = 0;

	poloygonLayout[3].SemanticName = "TANGENT";
	poloygonLayout[3].SemanticIndex = 0;
	poloygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[3].InputSlot = 0;
	poloygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[3].InstanceDataStepRate = 0;

	poloygonLayout[4].SemanticName = "TEXCOORD";
	poloygonLayout[4].SemanticIndex = 0;
	poloygonLayout[4].Format = DXGI_FORMAT_R32G32_FLOAT;
	poloygonLayout[4].InputSlot = 0;
	poloygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[4].InstanceDataStepRate = 0;

	UINT numElements = sizeof(poloygonLayout) / sizeof(poloygonLayout[0]);

	// 创建顶点输入布局。
	HR(mDevice->CreateInputLayout(poloygonLayout, numElements, vsShaderData, vsShaderSize, &mInputLayout));

	delete[] vsShaderData;
	shaderFile.close();

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建const buffer指针，以便访问shader常量。
	HR(mDevice->CreateBuffer(&matrixBufferDesc, nullptr, &mMatrixBuffer));

	D3D11_BUFFER_DESC testBufferDesc;
	ZeroMemory(&testBufferDesc, sizeof(D3D11_BUFFER_DESC));
	testBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	testBufferDesc.ByteWidth = sizeof(TestBuffer);
	testBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	testBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	testBufferDesc.MiscFlags = 0;
	testBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&testBufferDesc, nullptr, &mTestBuffer));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerDesc, &mSamplerState));

	renderShader();

	return true;
}

bool Shader::render(RenderParameters renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	if (!setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	return true;
}

bool Shader::setShaderParameters(RenderParameters renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* matrixData;
	UINT bufferNumber;

	// 传入Shader前，确保矩阵转置，这是D3D11的要求。
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓冲，以便能够写入。
	HR(mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	// 得到const buffer指针。
	matrixData = (MatrixBuffer*)mappedResource.pData;

	// 设置world，view和projection矩阵。
	XMStoreFloat4x4(&matrixData->worldMatrix, worldMatrixTemp);
	XMStoreFloat4x4(&matrixData->viewMatrix, viewMatrixTemp);
	XMStoreFloat4x4(&matrixData->projectionMatrix, projectionMatrixTemp);

#if USE_RIGHT_HAND
	matrixData->lightPosition = XMFLOAT4(0.0, 5.0f, 5.0f, 1.0f);
#else
	matrixData->lightPosition = XMFLOAT4(0.0, 5.0f, -5.0f, 1.0f);
#endif

	matrixData->diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
#if USE_RIGHT_HAND
	matrixData->cameraPositon = XMFLOAT4(0.0f, 0.0f, 10.0f, 1.0f);
#else
	matrixData->cameraPositon = XMFLOAT4(0.0f, 0.0f, -10.0f, 1.0f);
#endif
	matrixData->specularColor = XMFLOAT4(Colors::White);

	// 解锁常量缓冲。
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	TestBuffer* testData;
	ZeroMemory(&mappedResource, sizeof D3D11_MAPPED_SUBRESOURCE);
	HR(mDeviceContext->Map(mTestBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	testData = (TestBuffer*)mappedResource.pData;

	testData->scaleFactor = 0.5f;
	testData->scaleFactor1 = 0.5f;
	testData->scaleFactor2 = 0.5f;
	testData->scaleFactor3 = 0.5f;
	testData->hasDiffuseTexture = renderParameters.hasDiffuseTexture;
	testData->hasNormalMapTexture = renderParameters.hasNormalMapTexture;

	mDeviceContext->Unmap(mTestBuffer, 0);

	// 设置常量缓冲位置。
	bufferNumber = 0;

	// 用更新后的值设置常量缓冲。
	ID3D11Buffer* buffers[] = { mMatrixBuffer, mTestBuffer };
	mDeviceContext->VSSetConstantBuffers(bufferNumber, 2, buffers);
	mDeviceContext->PSSetConstantBuffers(bufferNumber, 2, buffers);

	return true;
}

void Shader::renderShader()
{
	// 绑定顶点布局。
	mDeviceContext->IASetInputLayout(mInputLayout);

	// 设置渲染使用VS和PS。
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
}

void Shader::setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews)
{
	mDeviceContext->PSSetShaderResources(0, numViews, ppShaderResourceViews);
}

void Shader::shutdown()
{
	SafeDelete(mSamplerState);
	SafeDelete(mPixelShader);
	SafeDelete(mVertexShader);
	SafeDelete(mInputLayout);
	SafeDelete(mMatrixBuffer);
}