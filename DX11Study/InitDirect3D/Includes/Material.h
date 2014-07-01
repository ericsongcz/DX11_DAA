#pragma once
#include <string>
#include <d3d11.h>

using std::string;

class Material
{
public:
	Material();
	Material(int id, string diffuse, string normalMap);
	~Material();

	void setDiffuseTexture(string diffuseTextureFile);
	void setNormalTexture(string normalMapTextureFile);
	bool hasDiffuseTexture() const;
	bool hasNormalMapTexture() const;
	string getDiffuseTextureFile() const;
	string getNormalMapTextureFile() const;
	ID3D11ShaderResourceView* getDiffuseTexture() const;
	ID3D11ShaderResourceView* getNormalMapTexture() const;
	void clear();

	int materialId;
	string mDiffuseTextureFile;
	string mNormalMapTextureFile;

	ID3D11ShaderResourceView* mDiffuseTexture;
	ID3D11ShaderResourceView* mNormalMapTexture;
	bool mHasDiffuseTexture;
	bool mHasNormalMapTexture;
};

struct MaterialIdOffset
{
	MaterialIdOffset()
		: polygonCount(0),
		material(new Material())
	{}

	int polygonCount;
	Material* material;
};