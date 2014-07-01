#include "stdafx.h"
#include "Material.h"
#include "D3DUtils.h"

Material::Material()
{

}

Material::Material(int id, string diffuse, string normalMap)
	: materialId(id),
	diffuseTextureFile(diffuse),
	normalMapTextureFile(normalMap)
{}

void Material::setDiffuseTexture(string diffuse)
{
	diffuseTexture = CreateShaderResourceViewFromFile()
}

void Material::setNormalTexture(string normalMap)
{

}
