#include "stdafx.h"
#include "ShaderUntils.h"
#include <fstream>

ShaderData* createVertexAndPixelShader(ID3D11Device* device, const wchar_t* vertexShaderFileName, const wchar_t* pixelShaderFileName)
{
	ShaderData* shader = new ShaderData();

	// 从磁盘加载编译好的Shader源文件(.cso)。
	std::ifstream shaderFile;
	size_t shaderSize = 0;
	char* shaderData;

	shaderFile.open(vertexShaderFileName, std::ifstream::in | std::ifstream::binary);

	if (shaderFile.good())
	{
		shaderFile.seekg(0, std::ios::end);
		shaderSize = size_t(shaderFile.tellg());
		shaderData = new char[shaderSize];
		shaderFile.seekg(0, std::ios::beg);
		shaderFile.read(&shaderData[0], shaderSize);
	}
	else
	{
		return nullptr;
	}

	ID3D11VertexShader* vertexShader = nullptr;

	HR(device->CreateVertexShader(shaderData, shaderSize, nullptr, &vertexShader));

	shader->vertexShader = vertexShader;
	shader->vertexShaderSize = shaderSize;
	shader->vertexShaderData = shaderData;

	shaderFile.close();

	shaderFile.open(pixelShaderFileName, std::ifstream::in | std::ifstream::binary);

	if (shaderFile.good())
	{
		shaderFile.seekg(0, std::ios::end);
		shaderSize = size_t(shaderFile.tellg());
		shaderData = new char[shaderSize];
		shaderFile.seekg(0, std::ios::beg);
		shaderFile.read(&shaderData[0], shaderSize);
	}
	else
	{
		return nullptr;
	}

	ID3D11PixelShader* pixelShader = nullptr;

	HR(device->CreatePixelShader(shaderData, shaderSize, nullptr, &pixelShader));

	shader->pixelShader = pixelShader;
	shader->pixelShaderSize = shaderSize;
	shader->pixelShaderData = shaderData;

	shaderFile.close();

	return shader;
}
