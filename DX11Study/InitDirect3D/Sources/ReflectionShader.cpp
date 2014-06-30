#include "stdafx.h"
#include "ReflectionShader.h"
#include "D3DUtils.h"
#include "SharedParameters.h"

ReflectionShader::ReflectionShader()
	: mMatrixBuffer(nullptr),
	mReflectionBuffer(nullptr),
	mInputLayout(nullptr),
	mVertexShader(nullptr),
	mPixelShader(nullptr),
	mSamplerStateLinear(nullptr),
	mSamplerStateAnisotropic(nullptr),
	mDeviceContext(nullptr),
	mShaderResourceView(nullptr)
{

}

ReflectionShader::~ReflectionShader()
{
	shutdown();
}

bool ReflectionShader::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* vsFileName, const wchar_t* psFileName)
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

	D3D11_BUFFER_DESC reflectionBufferDesc;
	ZeroMemory(&reflectionBufferDesc, sizeof(D3D11_BUFFER_DESC));
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBuffer);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	HR(mDevice->CreateBuffer(&reflectionBufferDesc, nullptr, &mReflectionBuffer));

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

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerLinearDesc, &mSamplerStateLinear));

	D3D11_SAMPLER_DESC samplerAnisotropicDesc;
	ZeroMemory(&samplerAnisotropicDesc, sizeof(D3D11_SAMPLER_DESC));

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

	// 创建纹理采样状态。
	HR(mDevice->CreateSamplerState(&samplerAnisotropicDesc, &mSamplerStateAnisotropic));

	return true;
}

bool ReflectionShader::render(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	renderShader();
	setShaderParameters(renderParameters, worldMatrix, viewMatrix, projectionMatrix);

	return true;
}

bool ReflectionShader::setShaderParameters(RenderParameters& renderParameters, FXMMATRIX& worldMatrix, FXMMATRIX& viewMatrix, FXMMATRIX& projectionMatrix)
{
	// 传入Shader前，确保矩阵转置，这是D3D11的要求。
	XMMATRIX worldViewProjection = XMMatrixMultiply(worldMatrix, viewMatrix);
	worldViewProjection = XMMatrixMultiply(worldViewProjection, projectionMatrix);

	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	XMMATRIX worldMatrixTemp = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMatrixTemp = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionMatrixTemp = XMMatrixTranspose(projectionMatrix);
	XMMATRIX reflectionMatrixTemp = XMMatrixTranspose(XMLoadFloat4x4(&renderParameters.reflectionMatrix));

	SharedParameters::worldMatrix = worldMatrixTemp;

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

	// 解锁常量缓冲。
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE reflectionBufferResource;

	ReflectionBuffer* reflectionBufferData;

	HR(mDeviceContext->Map(mReflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &reflectionBufferResource));

	reflectionBufferData = (ReflectionBuffer*)reflectionBufferResource.pData;

	XMStoreFloat4x4(&reflectionBufferData->reflectionMatrix, reflectionMatrixTemp);

	mDeviceContext->Unmap(mReflectionBuffer, 0);

	// 设置常量缓冲位置。
	UINT startSlot = 0;

	// 用更新后的值设置常量缓冲。
	ID3D11Buffer* buffers[] = { mMatrixBuffer, mReflectionBuffer };
	mDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

	return true;
}

void ReflectionShader::renderShader()
{
	// 绑定顶点布局。
	mDeviceContext->IASetInputLayout(mInputLayout);

	// 设置渲染使用VS和PS。
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	setSamplerState(SharedParameters::samplerType);
}

void ReflectionShader::setSamplerState(ESamplerType samplerType)
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


void ReflectionShader::shutdown()
{
	SafeRelease(mSamplerStateLinear);
	SafeRelease(mSamplerStateAnisotropic);
	SafeRelease(mPixelShader);
	SafeRelease(mVertexShader);
	SafeRelease(mInputLayout);
	SafeRelease(mMatrixBuffer);
}

void ReflectionShader::setShaderResource(ID3D11ShaderResourceView *const *ppShaderResourceViews, int numViews)
{
	mDeviceContext->PSSetShaderResources(0, numViews, ppShaderResourceViews);
}