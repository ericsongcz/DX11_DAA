#include "stdafx.h"
#include "Shader.h"
#include "D3DUtils.h"
#include <fstream>

Shader::Shader()
: mMatrixBuffer(nullptr),
  mInputLayout(nullptr),
  mVertexShader(nullptr),
  mPixelShader(nullptr),
  mSamplerState(nullptr),
  mDeviceContext(nullptr),
  mShaderResourceView(nullptr)
{

}

Shader::~Shader()
{

}

bool Shader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	// �Ӵ��̼��ر���õ�Vertex ShaderԴ�ļ���
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

	if (FAILED(mDevice->CreateVertexShader(vsShaderData, vsShaderSize, nullptr, &mVertexShader)))
	{
		return false;
	}

	// ���ڶ����ļ�֮ǰ��Ҫclose��
	shaderFile.close();

	// �Ӵ��̼��ر���õ�Pixel ShaderԴ�ļ���
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

	if (FAILED(mDevice->CreatePixelShader(psShaderData, psShaderSize, nullptr, &mPixelShader)))
	{
		return false;
	}

	delete[] psShaderData;

	// �������ݲ��֣��Ա���Shader��ʹ�á�
	// ����Ҫ�Ͷ���ṹһ�¡�
	D3D11_INPUT_ELEMENT_DESC poloygonLayout[2];
	ZeroMemory(&poloygonLayout[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
	ZeroMemory(&poloygonLayout[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
	//ZeroMemory(&poloygonLayout[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
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

	//poloygonLayout[2].SemanticName = "TEXCOORD";
	//poloygonLayout[2].SemanticIndex = 0;
	//poloygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	//poloygonLayout[2].InputSlot = 0;
	//poloygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//poloygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//poloygonLayout[2].InstanceDataStepRate = 0;

	UINT numElements = sizeof(poloygonLayout) / sizeof(poloygonLayout[0]);

	// �����������벼�֡�
	if (FAILED(mDevice->CreateInputLayout(poloygonLayout, numElements, vsShaderData, vsShaderSize, &mInputLayout)))
	{
		return false;
	}

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

	// ����const bufferָ�룬�Ա����shader������
	if (FAILED(mDevice->CreateBuffer(&matrixBufferDesc, nullptr, &mMatrixBuffer)))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// ������������״̬��
	HRESULT hr = mDevice->CreateSamplerState(&samplerDesc, &mSamplerState);

	if (FAILED(hr))
	{
		return false;
	}

	renderShader();

	return true;
}

bool Shader::render(FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	if (!setShaderParameters(worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	return true;
}

bool Shader::setShaderParameters(FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* data;
	UINT bufferNumber;

	// ����Shaderǰ��ȷ������ת�ã�����D3D11��Ҫ��
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);

	// �����������壬�Ա��ܹ�д�롣
	HR(mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	// �õ�const bufferָ�롣
	data = (MatrixBuffer*)mappedResource.pData;

	// ����world��view��projection����
	XMStoreFloat4x4(&data->worldMatrix, worldMatrixTemp);
	XMStoreFloat4x4(&data->viewMatrix, viewMatrixTemp);
	XMStoreFloat4x4(&data->projectionMatrix, projectionMatrixTemp);
	XMVECTOR color = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMStoreFloat4(&data->color1, color);
	XMStoreFloat4(&data->color2, color);
	XMStoreFloat4(&data->color3, color);
	XMStoreFloat4(&data->color4, color);

	// �����������塣
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	// ���ó�������λ�á�
	bufferNumber = 0;

	// �ø��º��ֵ���ó������塣
	mDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);
	mDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &mMatrixBuffer);

	return true;
}

void Shader::renderShader()
{
	// �󶨶��㲼�֡�
	mDeviceContext->IASetInputLayout(mInputLayout);

	// ������Ⱦʹ��VS��PS��
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);
}

void Shader::setShaderResource(ID3D11ShaderResourceView* shaderResourceView)
{
	mDeviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
}

void Shader::shutdown()
{
	SafeDelete(mSamplerState);
	SafeDelete(mPixelShader);
	SafeDelete(mVertexShader);
	SafeDelete(mInputLayout);
	SafeDelete(mMatrixBuffer);
}