#include "stdafx.h"
#include "LightShader.h"
#include "D3DUtils.h"
#include "SharedParameters.h"

LightShader::LightShader()
	: mMatrixBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
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

	// �������ݲ��֣��Ա���Shader��ʹ�á�
	// ����Ҫ�Ͷ���ṹһ�¡�
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[2];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));

	poloygonLayout[0].SemanticName = "POSITION";	// VS�е����������
	poloygonLayout[0].SemanticIndex = 0;
	poloygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	poloygonLayout[0].InputSlot = 0;
	poloygonLayout[0].AlignedByteOffset = 0;
	poloygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[0].InstanceDataStepRate = 0;

	poloygonLayout[1].SemanticName = "TEXCOORD";
	poloygonLayout[1].SemanticIndex = 0;
	poloygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	poloygonLayout[1].InputSlot = 0;
	poloygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	poloygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	poloygonLayout[1].InstanceDataStepRate = 0;

	UINT numElements = sizeof(poloygonLayout) / sizeof(poloygonLayout[0]);

	// �����������벼�֡�
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

	// ����constant bufferָ�룬�Ա����shader������
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

	D3D11_BUFFER_DESC fogBufferDesc;
	ZeroMemory(&fogBufferDesc, sizeof(D3D11_BUFFER_DESC));

	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBuffer);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&fogBufferDesc, nullptr, &mFogBuffer));

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
	// ����Shaderǰ��ȷ������ת�ã�����D3D11��Ҫ��
	XMMATRIX worldViewProjection = XMMatrixMultiply(worldMatrix, viewMatrix);
	worldViewProjection = XMMatrixMultiply(worldViewProjection, projectionMatrix);

	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);

	MatrixBuffer* matrixBufferData = nullptr;

	D3D11_MAPPED_SUBRESOURCE matrixBufferResource;
	ZeroMemory(&matrixBufferResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// �����������壬�Ա��ܹ�д�롣
	HR(mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matrixBufferResource));

	// �õ�const bufferָ�롣
	matrixBufferData = (MatrixBuffer*)matrixBufferResource.pData;

	// ����world��view��projection����
	XMStoreFloat4x4(&matrixBufferData->worldMatrix, worldMatrixTemp);
	XMStoreFloat4x4(&matrixBufferData->viewMatrix, viewMatrixTemp);
	XMStoreFloat4x4(&matrixBufferData->projectionMatrix, projectionMatrixTemp);
	XMStoreFloat4x4(&matrixBufferData->worldViewProjectionMatrix, worldViewProjection);

	// �����������塣
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	LightBuffer* lightBufferData = nullptr;
	D3D11_MAPPED_SUBRESOURCE lightBufferResource;
	ZeroMemory(&lightBufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

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
	pointLight.position = XMFLOAT4(0.0, 10.0f, 5.0f, 1.0f);
	pointLight.radius = 50.0f;
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

	// ���ó�������λ�á�
	UINT startSlot = 0;

	// �ø��º��ֵ���ó������塣
	ID3D11Buffer* constantBuffers[] = { mMatrixBuffer, mLightBuffer, mFogBuffer};
	mDeviceContext->VSSetConstantBuffers(startSlot, 1, &mMatrixBuffer);
	mDeviceContext->PSSetConstantBuffers(startSlot, 1, &mLightBuffer);
	mDeviceContext->PSSetConstantBuffers(startSlot + 1, 1, &mFogBuffer);

	return true;
}

void LightShader::renderShader()
{
	// �󶨶��㲼�֡�
	mDeviceContext->IASetInputLayout(mInputLayout);

	// ������Ⱦʹ��VS��PS��
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
}

void LightShader::shutdown()
{
	SafeDelete(mPixelShader);
	SafeDelete(mVertexShader);
	SafeDelete(mInputLayout);
	SafeDelete(mMatrixBuffer);
}
