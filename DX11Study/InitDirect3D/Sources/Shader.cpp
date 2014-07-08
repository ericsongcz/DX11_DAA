#include "stdafx.h"
#include "Shader.h"
#include "SharedParameters.h"

Shader::Shader()
	: mMatrixBuffer(nullptr),
	mLightBuffer(nullptr),
	mCommonBuffer(nullptr),
	mFogBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
	mDeviceContext(nullptr)
{
	SharedParameters::shader = this;
}

Shader::~Shader()
{
	shutdown();
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
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[6];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[3], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[4], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[5], sizeof(D3D11_INPUT_ELEMENT_DESC));

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

	poloygonLayout[4].SemanticName = "BINORMAL";
	poloygonLayout[4].SemanticIndex = 0;
	poloygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[4].InputSlot = 0;
	poloygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[4].InstanceDataStepRate = 0;

	poloygonLayout[5].SemanticName = "TEXCOORD";
	poloygonLayout[5].SemanticIndex = 0;
	poloygonLayout[5].Format = DXGI_FORMAT_R32G32_FLOAT;
	poloygonLayout[5].InputSlot = 0;
	poloygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[5].InstanceDataStepRate = 0;

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

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&lightBufferDesc, nullptr, &mLightBuffer));

	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(D3D11_BUFFER_DESC));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CommonBuffer);
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	commonBufferDesc.MiscFlags = 0;
	commonBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&commonBufferDesc, nullptr, &mCommonBuffer));

	D3D11_BUFFER_DESC fogBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(D3D11_BUFFER_DESC));
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBuffer);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&fogBufferDesc, nullptr, &mFogBuffer));

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
	XMMATRIX textureTransformMatrixTemp = XMMatrixTranspose(XMLoadFloat4x4(&renderParameters.textureTransformMatrix));

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
	XMStoreFloat4x4(&matrixData->worldViewProjectionMatrix, worldViewProjection);
	XMStoreFloat4x4(&matrixData->textureTransformMatrix, textureTransformMatrixTemp);

	// 解锁常量缓冲。
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE lightBufferResource;
	LightBuffer* lightBufferData;

	HR(mDeviceContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightBufferResource));

	lightBufferData = (LightBuffer*)lightBufferResource.pData;

	lightBufferData->ambientColor = renderParameters.ambientColor;
	lightBufferData->ambientIntensity = renderParameters.ambientIntensity;
	lightBufferData->diffuseIntensity = renderParameters.diffuseIntensity;
	lightBufferData->pad1 = 0.0f;
	lightBufferData->pad2 = 0.0f;

	XMFLOAT3 cameraPosition = SharedParameters::camera->getPosition();
	lightBufferData->cameraPositon = XMFLOAT4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);

	lightBufferData->specularColor = XMFLOAT4(Colors::White);

	DirectionalLight directionalLight;
	directionalLight.diffuseColor = renderParameters.diffuseColor;
	directionalLight.direction = XMFLOAT4(-1.0f, -1.0f, 0.0f, 0.0f);
	lightBufferData->directionalLight = directionalLight;

	PointLight pointLight;
	pointLight.diffuseColor = renderParameters.diffuseColor;
	pointLight.position = renderParameters.pointLightPosition;
	pointLight.radius = 30.0f;
	pointLight.attenuation0 = 0.0f;
	pointLight.attenuation1 = 1.0f;
	pointLight.attenuation2 = 0.0f;
	lightBufferData->pointLight = pointLight;

	Spotlight spotLight;
	spotLight.diffuseColor = renderParameters.diffuseColor;;
	spotLight.position = XMFLOAT3(0.0f, 5.0f, 0.0f);
	spotLight.radius = 10.0f;
	spotLight.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	spotLight.spot = 8.0f;
	lightBufferData->spotLight = spotLight;

	mDeviceContext->Unmap(mLightBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE commonBufferResource;

	CommonBuffer* commonBufferData;

	HR(mDeviceContext->Map(mCommonBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &commonBufferResource));

	commonBufferData = (CommonBuffer*)commonBufferResource.pData;

	commonBufferData->hasDiffuseTexture = renderParameters.hasDiffuseTexture;
	commonBufferData->hasNormalMapTexture = renderParameters.hasNormalMapTexture;
	commonBufferData->showDepthComplexity = renderParameters.showDepthComplexity;

	// 是否有漫反射纹理。
	if (renderParameters.hasDiffuseTexture)
	{
		commonBufferData->factor = 1.0f;
	}
	else
	{
		commonBufferData->factor = 0.0f;
	}

	// 是否有法线贴图。
	if (renderParameters.hasNormalMapTexture)
	{
		commonBufferData->index = 1;
	}
	else
	{
		commonBufferData->index = 0;
	}

	mDeviceContext->Unmap(mCommonBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE fogBufferResource;
	FogBuffer* fogBufferData;

	HR(mDeviceContext->Map(mFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &fogBufferResource));

	fogBufferData = (FogBuffer*)fogBufferResource.pData;
	fogBufferData->fogColor = renderParameters.fogColor;
	fogBufferData->fogDensity = renderParameters.fogDensity;
	fogBufferData->fogStart = renderParameters.fogStart;
	fogBufferData->fogRange = renderParameters.fogRange;
	fogBufferData->fogType = renderParameters.fogType;
	fogBufferData->showFog = renderParameters.showFog;

	mDeviceContext->Unmap(mFogBuffer, 0);

	// 设置常量缓冲位置。
	UINT startSlot = 0;

	// 用更新后的值设置常量缓冲。
	ID3D11Buffer* buffers[] = { mMatrixBuffer, mLightBuffer, mCommonBuffer, mFogBuffer };
	mDeviceContext->VSSetConstantBuffers(0, 3, buffers);
	mDeviceContext->PSSetConstantBuffers(0, 4, buffers);

	return true;
}

void Shader::renderShader()
{
	// 绑定顶点布局。
	mDeviceContext->IASetInputLayout(mInputLayout);

	// 设置渲染使用VS和PS。
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
}

void Shader::shutdown()
{
	SafeRelease(mPixelShader);
	SafeRelease(mVertexShader);
	SafeRelease(mInputLayout);
	SafeRelease(mFogBuffer);
	SafeRelease(mCommonBuffer);
	SafeRelease(mLightBuffer);
	SafeRelease(mMatrixBuffer);
}
