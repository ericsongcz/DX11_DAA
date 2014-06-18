#pragma once

#include "../Utils/Base.h"

#include <string>

ANNA_NAMESPACE_BEGIN

class ANNA_CORE_API Shader
{
public:
	enum ShaderType
	{
		NULL_TYPE,
		VERTEX,
		FRAGMENT
	};

public:
	Shader();
	virtual ~Shader();

public:
	virtual void bind();
	virtual void unbind();
	virtual void compile() = 0;

public:
	void setVertexParameterByFloat(const std::string& strName, const float* pData, int nOffset, int nSize);
	void setVertexParameterByInt(const std::string& strName, const int* pData, int nOffset, int nSize);

	void setFragmentParameterByFloat(const std::string& strName, const float* pData, int nOffset, int nSize);
	void setFragmentParameterByInt(const std::string& strName, const int* pData, int nOffset, int nSize);

	bool loadShader(ShaderType type, const std::string& strFile);

	virtual void setVertexParameterByFloat(int nId, const float* pData, int nOffset, int nSize){};
	virtual void setVertexParameterByInt(int nId, const int* pData, int nOffset, int nSize){};

	virtual void setFragmentParameterByFloat(int nId, const float* pData, int nOffset, int nSize){};
	virtual void setFragmentParameterByInt(int nId, const int* pData, int nOffset, int nSize){};

public:
	void setParameterByFloat(ShaderType type, const std::string& strName, const float* pData, int nOffset, int nSize);
	void setParameterByInt(ShaderType type, const std::string& strName, const int* pData, int nOffset, int nSize);

public:
	virtual bool loadVertex(const std::string& strFile);
	virtual bool loadFragment(const std::string& strFile);
	virtual int findVertexParameter(const std::string& strName);
	virtual int findFragmentParameter(const std::string& strName);

public:
	virtual void addParameter(ShaderType type, const std::string& strName, int nParameterSize){};

public:
	void setVertexWorldViewProjectionParameters(const mat4& world, const mat4& view, const mat4& projection);

	static std::string getConstWorldViewProjectionParameterName()
	{
		return world_view_projection_parameter_name_;
	}

protected:
	virtual void bind_impl() = 0;
	virtual void unbind_impl() = 0;

private:
	static const std::string world_view_projection_parameter_name_;
};

ANNA_NAMESPACE_END