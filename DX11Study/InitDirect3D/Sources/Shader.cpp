#include "stdafx.h"
#include "Shader.h"
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

	ShaderData* shaderData = createVertexAndPixelShader(mDevice, vsFileName, psFileName);

	mVertexShader = shaderData->vertexShader;
	mPixelShader = shaderData->pixelShader;

	// 设置数据布局，以便在Shader中使用。
	// 定义要和顶点结构一致。
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[5];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[3], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[4], sizeof(D3D11_INPUT_ELEMENT_DESC));

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

	return true;
}

bool Shader::render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	renderShader();
	setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool Shader::setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
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

#if USE_RIGHT_HAND
	matrixData->lightPosition = XMFLOAT4(0.0f, 5.0f, 5.0f, 1.0f);
	matrixData->lightDirection = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
#else
	matrixData->lightPosition = XMFLOAT4(0.0, 5.0f, -5.0f, 1.0f);
#endif
	matrixData->ambientColor = renderParameters.ambientColor;
	matrixData->diffuseColor = renderParameters.diffuseColor;
	matrixData->ambientIntensity = renderParameters.ambientIntensity;
	matrixData->diffuseIntensity = renderParameters.diffuseIntensity;
	matrixData->pad1 = 0.0f;
	matrixData->pad2 = 0.0f;

	XMFLOAT3 cameraPosition = SharedParameters::camera->getPosition();
	matrixData->cameraPositon = XMFLOAT4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);

	matrixData->specularColor = XMFLOAT4(Colors::White);

	XMStoreFloat4x4(&matrixData->worldViewProjection, worldViewProjection);

	// 解锁常量缓冲。
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE testBufferResource;

	TestBuffer* testData;

	HR(mDeviceContext->Map(mTestBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &testBufferResource));

	testData = (TestBuffer*)testBufferResource.pData;

	testData->hasDiffuseTexture = renderParameters.hasDiffuseTexture;
	testData->hasNormalMapTexture = renderParameters.hasNormalMapTexture;
	
	// 是否有漫反射纹理。
	if (renderParameters.hasDiffuseTexture)
	{
		testData->factor = 1.0f;
	}
	else
	{
		testData->factor = 0.0f;
	}

	// 是否有法线贴图。
	if (renderParameters.hasNormalMapTexture)
	{
		testData->index = 1;
	}
	else
	{
		testData->index = 0;
	}

	PointLight pointLight;
	pointLight.lightPosition = XMFLOAT4(0.0, 5.0f, 5.0f, 1.0f);
	testData->pointLight = pointLight;

	mDeviceContext->Unmap(mTestBuffer, 0);

	// 设置常量缓冲位置。
	UINT startSlot = 0;

	// 用更新后的值设置常量缓冲。
	ID3D11Buffer* buffers[] = { mMatrixBuffer, mTestBuffer };
	mDeviceContext->VSSetConstantBuffers(0, 2, buffers);
	mDeviceContext->PSSetConstantBuffers(0, 2, buffers);

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