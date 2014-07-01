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
	void setNormalMapTexture(string normalMapTextureFile);
	void setDiffuseTexture(ID3D11ShaderResourceView* diffuseTexture);
	void setNormalMapTexture(ID3D11ShaderResourceView* normalMapTexture);
	bool hasDiffuseTexture() const;
	bool hasNormalMapTexture() const;
	string getDiffuseTextureFile() const;
	string getNormalMapTextureFile() const;
	ID3D11ShaderResourceView* getTexture(string textureFileName);
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
	map<string, ID3D11ShaderResourceView*> mTextureCache;
};

struct MaterialIdOffset
{
	MaterialIdOffset()
		: polygonCount(0)
	{}

	~MaterialIdOffset()
	{

	}

	int polygonCount;
	Material* material;
};