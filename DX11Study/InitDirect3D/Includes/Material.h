#pragma once
#include <string>
#include <d3d11.h>

using std::string;

class Material
{
public:
	Material() {}
	Material(int id, string diffuse, string normalMap);

	void setDiffuseTexture(string diffuse);
	void setNormalTexture(string normalMap);

private:
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