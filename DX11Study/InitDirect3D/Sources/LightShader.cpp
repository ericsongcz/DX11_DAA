#include "stdafx.h"
#include "LightShader.h"
#include "D3DUtils.h"
#include "SharedParameters.h"

LightShader::LightShader()
	: mMatrixBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
	mSamplerState(nullptr),
	mDeviceContext(nullptr),
	mShaderResourceView(nullptr)
{

}

LightShader::~LightShader()
{

}

bool LightShader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	ShaderData* shaderData = createVertexAndPixelShader(mDevice, vsFileName, psFileName);

	mVertexShader = shaderData->vertexShader;
	mPixelShader = shaderData->pixelShader;

	// 设置数据布局，以便在Shader中使用。
	// 定义要和顶点结构一致。
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[2];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));

	poloygonLayout[0].SemanticName = "POSITION";	// VS中的输入参数。
	poloygonLayout[0].SemanticIndex = 0;
	poloygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[0].InputSlot = 0;
	poloygonLayout[0].AlignedByteOffset = 0;
	poloygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[0].InstanceDataStepRate = 0;

	poloygonLayout[1].SemanticName = "TEXCOORD";
	poloygonLayout[1].SemanticIndex = 0;
	poloygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[1].InputSlot = 0;
	poloygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[1].InstanceDataStepRate = 0;

	UINT numElements = sizeof(poloygonLayout) / sizeof(poloygonLayout[0]);

	// 创建顶点输入布局。
	HR(mDevice->CreateInputLayout(poloygonLayout, numElements, shaderData->vertexShaderData, shaderData->vertexShaderSize, &mInputLayout));

	delete shaderData;

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建constant buffer指针，以便访问shader常量。
	HR(mDevice->CreateBuffer(&matrixBufferDesc, nullptr, &mMatrixBuffer));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
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

	return true;
}

bool LightShader::render(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	renderShader();
	setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool LightShader::setShaderParameters(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	// 传入Shader前，确保矩阵转置，这是D3D11的要求。
	XMMATRIX worldViewProjection = XMMatrixMultiply(worldMatrix, viewMatrix);
	worldViewProjection = XMMatrixMultiply(worldViewProjection, projectionMatrix);

	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);

	MatrixBuffer* matrixData;

	D3D11_MAPPED_SUBRESOURCE matrixBufferResource;

	// 锁定常量缓冲，以便能够写入。
	HR(mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matrixBufferResource));

	// 得到const buffer指针。
	matrixData = (MatrixBuffer*)matrixBufferResource.pData;

	// 设置world，view和projection矩阵。
	XMStoreFloat4x4(&matrixData->worldMatrix, worldMatrixTemp);
	XMStoreFloat4x4(&matrixData->viewMatrix, viewMatrixTemp);
	XMStoreFloat4x4(&matrixData->projectionMatrix, projectionMatrixTemp);
	XMStoreFloat4x4(&matrixData->worldViewProjection, worldViewProjection);

	matrixData->lightDirection = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);

	// 解锁常量缓冲。
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	// 设置常量缓冲位置。
	UINT startSlot = 0;

	// 用更新后的值设置常量缓冲。
	ID3D11Buffer* buffers[] = { mMatrixBuffer };
	mDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
	mDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

	return true;
}

void LightShader::renderShader()
{
	// 绑定顶点布局。
	mDeviceContext->IASetInputLayout(mInputLayout);

	// 设置渲染使用VS和PS。
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
}

void LightShader::shutdown()
{
	SafeDelete(mSamplerState);
	SafeDelete(mPixelShader);
	SafeDelete(mVertexShader);
	SafeDelete(mInputLayout);
	SafeDelete(mMatrixBuffer);
}

void LightShader::setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews)
{
	mDeviceContext->PSSetShaderResources(0, numViews, ppShaderResourceViews);
}
