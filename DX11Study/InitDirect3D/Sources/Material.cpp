#include "stdafx.h"
#include "Material.h"
#include "D3DUtils.h"
#include "SharedD3DDevice.h"

map<string, ID3D11ShaderResourceView*> Material::mTextureCache = map<string, ID3D11ShaderResourceView*>();

Material::Material()
	: materialId(0),
	mDiffuseTextureFile(""),
	mNormalMapTextureFile(""),
	mDiffuseTexture(nullptr),
	mNormalMapTexture(nullptr),
	mHasDiffuseTexture(false),
	mHasNormalMapTexture(false)
{

}

Material::Material(int id, string diffuse, string normalMap)
	: materialId(id),
	mDiffuseTextureFile(diffuse),
	mNormalMapTextureFile(normalMap),
	mDiffuseTexture(nullptr),
	mNormalMapTexture(nullptr)
{}

Material::~Material()
{
	clear();
}

void Material::setDiffuseTexture(string diffuseTextureFile)
{
	mDiffuseTextureFile = diffuseTextureFile;
	mDiffuseTexture = getTexture(diffuseTextureFile);
	mHasDiffuseTexture = true;
}

void Material::setNormalMapTexture(string normalMapTextureFile)
{
	mNormalMapTextureFile = normalMapTextureFile;
	mNormalMapTexture = getTexture(normalMapTextureFile);
	mHasNormalMapTexture = true;
}

void Material::setDiffuseTexture(ID3D11ShaderResourceView* diffuseTexture)
{
	mDiffuseTexture = diffuseTexture;
}

void Material::setNormalMapTexture(ID3D11ShaderResourceView* normalMapTexture)
{
	mNormalMapTexture = normalMapTexture;
}

bool Material::hasDiffuseTexture() const
{
	return mHasDiffuseTexture;
}

bool Material::hasNormalMapTexture() const
{
	return mHasNormalMapTexture;
}

string Material::getDiffuseTextureFile() const
{
	return mDiffuseTextureFile;
}

string Material::getNormalMapTextureFile() const
{
	return mNormalMapTextureFile;
}

ID3D11ShaderResourceView* Material::getTexture(string textureFileName)
{
	auto iter = mTextureCache.find(textureFileName);

	if (iter != mTextureCache.end())
	{
		return iter->second;
	}
	else
	{
		ID3D11ShaderResourceView* srv = CreateShaderResourceViewFromFile(textureFileName, SharedD3DDevice::device);
		mTextureCache[textureFileName] = srv;
		return srv;
	}

	return nullptr;
}

ID3D11ShaderResourceView* Material::getDiffuseTexture() const
{
	return mDiffuseTexture;
}

ID3D11ShaderResourceView* Material::getNormalMapTexture() const
{
	return mNormalMapTexture;
}

void Material::clear()
{
	SafeRelease(mNormalMapTexture);
	SafeRelease(mDiffuseTexture);
}