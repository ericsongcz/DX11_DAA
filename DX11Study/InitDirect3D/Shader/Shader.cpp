#include "Shader.h"
#include "../Utils/Log.h"
#include "RenderManager.h"

ANNA_NAMESPACE_BEGIN

const std::string Shader::world_view_projection_parameter_name_ = "WorldViewProjection";

Shader::Shader()
{

}

Shader::~Shader()
{

}

bool Shader::loadVertex(const std::string& strFile)
{
	return false;
}

bool Shader::loadFragment(const std::string& strFile)
{
	return false;
}

void Shader::setVertexParameterByFloat(const std::string& strName, const float* pData, int nOffset, int nSize)
{
	int nId = findVertexParameter(strName);

	if (nId == -1)
	{
		Log::error("Shader::setVertexParameterByFloat can not find parameter:%s", strName.c_str());
		return;
	}

	setVertexParameterByFloat(nId, pData, nOffset, nSize);
}

void Shader::setVertexParameterByInt(const std::string& strName, const int* pData, int nOffset, int nSize)
{
	int nId = findVertexParameter(strName);

	if (nId == -1)
	{
		Log::error("Shader::setVertexParameterByFloat can not find parameter:%s", strName.c_str());
		return;
	}

	setVertexParameterByInt(nId, pData, nOffset, nSize);
}

int Shader::findFragmentParameter(const std::string& strName)
{
	return -1;
}

int Shader::findVertexParameter(const std::string& strName)
{
	return -1;
}

void Shader::setFragmentParameterByFloat(const std::string& strName, const float* pData, int nOffset, int nSize)
{
	int nId = findFragmentParameter(strName);

	if (nId == -1)
	{
		Log::error("Shader::setFragmentParameterByFloat can not find parameter:%s", strName.c_str());
		return;
	}

	setFragmentParameterByFloat(nId, pData, nOffset, nSize);
}

void Shader::setFragmentParameterByInt(const std::string& strName, const int* pData, int nOffset, int nSize)
{
	int nId = findFragmentParameter(strName);

	if (nId == -1)
	{
		Log::error("Shader::setFragmentParameterByFloat can not find parameter:%s", strName.c_str());
		return;
	}

	setFragmentParameterByInt(nId, pData, nOffset, nSize);
}

bool Shader::loadShader(ShaderType type, const std::string& strFile)
{
	if (type == VERTEX)
	{
		return loadVertex(strFile);
	}
	else if (type == FRAGMENT)
	{
		return loadFragment(strFile);
	}

	return false;
}

void Shader::setParameterByFloat(ShaderType type, const std::string& strName, const float* pData, int nOffset, int nSize)
{
	if (type == VERTEX)
	{
		setVertexParameterByFloat(strName, pData, nOffset, nSize);
	}
	else if (type == FRAGMENT)
	{
		setFragmentParameterByFloat(strName, pData, nOffset, nSize);
	}
}

void Shader::setParameterByInt(ShaderType type, const std::string& strName, const int* pData, int nOffset, int nSize)
{
	if (type == VERTEX)
	{
		setVertexParameterByInt(strName, pData, nOffset, nSize);
	}
	else if (type == FRAGMENT)
	{
		setFragmentParameterByInt(strName, pData, nOffset, nSize);
	}
}

void Shader::setVertexWorldViewProjectionParameters(const mat4& world, const mat4& view, const mat4& projection)
{
	if (findVertexParameter(world_view_projection_parameter_name_) == -1)
	{
		Log::error("Shader::setVertexWorldViewProjectionParameters error,can NOT found world view porjection parameter");
		return;
	}

	setParameterByFloat(Anna::Shader::VERTEX, world_view_projection_parameter_name_, world, 0, sizeof(Anna::mat4));
	setParameterByFloat(Anna::Shader::VERTEX, world_view_projection_parameter_name_, view, sizeof(Anna::mat4), sizeof(Anna::mat4));
	setParameterByFloat(Anna::Shader::VERTEX, world_view_projection_parameter_name_, projection, sizeof(Anna::mat4) * 2, sizeof(Anna::mat4));
}

void Shader::bind()
{
	bind_impl();
	//RenderManager::pointer()->getRenderState()->setShader(this);
}

void Shader::unbind()
{
	unbind_impl();
	//RenderManager::pointer()->getRenderState()->setShader(nullptr);
}



ANNA_NAMESPACE_END