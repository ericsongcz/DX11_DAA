#include "stdafx.h"
#include "Material.h"
#include "D3DUtils.h"
#include "SharedD3DDevice.h"

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
	mDiffuseTexture = CreateShaderResourceViewFromFile(diffuseTextureFile, SharedD3DDevice::device);
	mHasDiffuseTexture = true;
}

void Material::setNormalTexture(string normalMapTextureFile)
{
	mNormalMapTextureFile = normalMapTextureFile;
	mNormalMapTexture = CreateShaderResourceViewFromFile(normalMapTextureFile, SharedD3DDevice::device);
	mHasNormalMapTexture = true;
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
