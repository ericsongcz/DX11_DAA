#include "stdafx.h"
#include "DepthShader.h"
#include "SharedParameters.h"
#include "ShaderUntils.h"

DepthShader::DepthShader()
	: mMatrixBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
	mDeviceContext(nullptr)
{
}

DepthShader::~DepthShader()
{
	shutdown();
}

bool DepthShader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	ShaderData* shaderData = createVertexAndPixelShader(mDevice, vsFileName, psFileName);

	mVertexShader = shaderData->vertexShader;
	mPixelShader = shaderData->pixelShader;

	// �������ݲ��֣��Ա���Shader��ʹ�á�
	// ����Ҫ�Ͷ���ṹһ�¡�
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[6];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[3], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[4], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[5], sizeof(D3D11_INPUT_ELEMENT_DESC));

	poloygonLayout[0].SemanticName = "POSITION";	// VS�е����������
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

	return true;
}

bool DepthShader::render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	renderShader();
	setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool DepthShader::setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	// ����Shaderǰ��ȷ������ת�ã�����D3D11��Ҫ��
	XMMATRIX worldViewProjection = XMMatrixMultiply(worldMatrix, viewMatrix);
	worldViewProjection = XMMatrixMultiply(worldViewProjection, projectionMatrix);

	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);
	XMMATRIX textureTransformMatrixTemp = XMMatrixTranspose(XMLoadFloat4x4(&renderParameters.textureTransformMatrix));

	MatrixBuffer* matrixData;

	D3D11_MAPPED_SUBRESOURCE matrixBufferResource;

	// �����������壬�Ա��ܹ�д�롣
	HR(mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matrixBufferResource));

	// �õ�const bufferָ�롣
	matrixData = (MatrixBuffer*)matrixBufferResource.pData;

	// ����world��view��projection����
	XMStoreFloat4x4(&matrixData->worldMatrix, worldMatrixTemp);
	XMStoreFloat4x4(&matrixData->viewMatrix, viewMatrixTemp);
	XMStoreFloat4x4(&matrixData->projectionMatrix, projectionMatrixTemp);
	XMStoreFloat4x4(&matrixData->worldViewProjectionMatrix, worldViewProjection);
	XMStoreFloat4x4(&matrixData->textureTransformMatrix, textureTransformMatrixTemp);

	// �����������塣
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	// ���ó�������λ�á�
	UINT startSlot = 0;

	// �ø��º��ֵ���ó������塣
	ID3D11Buffer* buffers[] = { mMatrixBuffer };
	mDeviceContext->VSSetConstantBuffers(0, 1, buffers);

	return true;
}

void DepthShader::renderShader()
{
	// �󶨶��㲼�֡�
	mDeviceContext->IASetInputLayout(mInputLayout);

	// ������Ⱦʹ��VS��PS��
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
}


void DepthShader::shutdown()
{
	SafeRelease(mPixelShader);
	SafeRelease(mVertexShader);
	SafeRelease(mInputLayout);
	SafeRelease(mMatrixBuffer);
}