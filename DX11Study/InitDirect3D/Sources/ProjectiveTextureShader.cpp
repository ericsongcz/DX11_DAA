#include "stdafx.h"
#include "ProjectiveTextureShader.h"
#include "SharedParameters.h"

ProjectiveTextureShader::ProjectiveTextureShader()
	: mMatrixBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
	mSamplerStateLinear(nullptr),
	mSamplerStateAnisotropic(nullptr),
	mDeviceContext(nullptr),
	mShaderResourceView(nullptr)
{
}

ProjectiveTextureShader::~ProjectiveTextureShader()
{
	shutdown();
}

bool ProjectiveTextureShader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
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

	HR(mDevice->CreateBuffer(&matrixBufferDesc, nullptr, &mMatrixBuffer));

	D3D11_SAMPLER_DESC samplerLinearDesc;
	ZeroMemory(&samplerLinearDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerLinearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerLinearDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerLinearDesc.MipLODBias = 0.0f;
	samplerLinearDesc.MaxAnisotropy = 1;
	samplerLinearDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerLinearDesc.BorderColor[0] = 0;
	samplerLinearDesc.BorderColor[1] = 0;
	samplerLinearDesc.BorderColor[2] = 0;
	samplerLinearDesc.BorderColor[3] = 0;
	samplerLinearDesc.MinLOD = 0;
	samplerLinearDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �����������״̬��
	HR(mDevice->CreateSamplerState(&samplerLinearDesc, &mSamplerStateLinear));

	D3D11_SAMPLER_DESC samplerAnisotropicDesc;
	ZeroMemory(&samplerLinearDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerAnisotropicDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerAnisotropicDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerAnisotropicDesc.MipLODBias = 0.0f;
	samplerAnisotropicDesc.MaxAnisotropy = 16;
	samplerAnisotropicDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerAnisotropicDesc.BorderColor[0] = 0;
	samplerAnisotropicDesc.BorderColor[1] = 0;
	samplerAnisotropicDesc.BorderColor[2] = 0;
	samplerAnisotropicDesc.BorderColor[3] = 0;
	samplerAnisotropicDesc.MinLOD = 0;
	samplerAnisotropicDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �����������״̬��
	HR(mDevice->CreateSamplerState(&samplerAnisotropicDesc, &mSamplerStateAnisotropic));

	return true;
}

bool ProjectiveTextureShader::render(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	renderShader();
	setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool ProjectiveTextureShader::setShaderParameters(const RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	// ����Shaderǰ��ȷ������ת�ã�����D3D11��Ҫ��
	XMMATRIX worldViewProjection = XMMatrixMultiply(worldMatrix, viewMatrix);
	worldViewProjection = XMMatrixMultiply(worldViewProjection, projectionMatrix);

	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);
	XMMATRIX viewMatrixTemp2 = XMMatrixTranspose(XMLoadFloat4x4(&renderParameters.viewMatrix2));
	XMMATRIX projectionMatrixTemp2 = XMMatrixTranspose(XMLoadFloat4x4(&renderParameters.projectionMatrix2));
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
	XMStoreFloat4x4(&matrixData->viewMatrix2, viewMatrixTemp2);
	XMStoreFloat4x4(&matrixData->projectionMatrix2, projectionMatrixTemp2);

	// �����������塣
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	// ���ó�������λ�á�
	UINT startSlot = 0;

	// �ø��º��ֵ���ó������塣
	ID3D11Buffer* buffers[] = { mMatrixBuffer };
	mDeviceContext->VSSetConstantBuffers(0, 1, buffers);

	return true;
}

void ProjectiveTextureShader::renderShader()
{
	// �󶨶��㲼�֡�
	mDeviceContext->IASetInputLayout(mInputLayout);

	// ������Ⱦʹ��VS��PS��
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	setSamplerState(SharedParameters::samplerType);
}

void ProjectiveTextureShader::setSamplerState(ESamplerType samplerType)
{
	switch (samplerType)
	{
	case ST_LINEAR:
		mDeviceContext->PSSetSamplers(0, 1, &mSamplerStateLinear);
		break;

	case ST_ANISOTROPIC:
		mDeviceContext->PSSetSamplers(0, 1, &mSamplerStateAnisotropic);
		break;

	default:
		break;
	}
}

void ProjectiveTextureShader::shutdown()
{
	SafeRelease(mSamplerStateLinear);
	SafeRelease(mSamplerStateAnisotropic);
	SafeRelease(mPixelShader);
	SafeRelease(mVertexShader);
	SafeRelease(mInputLayout);
	SafeRelease(mMatrixBuffer);
}