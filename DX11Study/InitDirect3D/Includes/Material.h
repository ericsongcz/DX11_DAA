#pragma once

struct Material
{
	Material() {}
	Material(int id, string diffuse, string normalMap)
		: materialId(id),
		diffuseTextureFile(diffuse),
		normalMapTextureFile(normalMap)
	{}

	void setDiffuseTexture(string diffuse);
	void setNormalTexture(string normalMap);

	int materialId;
	string diffuseTextureFile;
	string normalMapTextureFile;

	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11ShaderResourceView* normalMapTexture;
};

struct MaterialIdOffset
{
	MaterialIdOffset()
		: polygonCount(0)
	{}
	int polygonCount;
	Material material;
};