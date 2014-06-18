#include "Shader.h"
#include "D3D11Helper.h"
#include "../Math/Math.h"

#include <map>
#include <vector>

ANNA_NAMESPACE_BEGIN

class ANNA_CORE_API D3D11Shader
	: public Shader
{
public:
	D3D11Shader();
	~D3D11Shader();

public:
	void compile();

	bool loadVertex(const std::string& file);
	bool loadFragment(const std::string& file);
	int findVertexParameter(const std::string& name);
	int findFragmentParameter(const std::string& name);

	void setVertexParameterByFloat(int id, const float* data, int offset, int size);
	void setVertexParameterByInt(int id, const int* data, int offset, int size);
	void setFragmentParameterByFloat(int id, const float* data, int offset, int size);
	void setFragmentParameterByInt(int id, const int* data, int offset, int size);

public:
	void addParameter(ShaderType type, const std::string& name, int parameter_size);

protected:
	void bind_impl();
	void unbind_impl();

private:
	bool compile(const std::string& file, const char* entry, const char* model, ID3DBlob** buffer);
	void setParameter(ShaderType type, int id, const int* data, int offset, int size);

	/**
	*@brief 整批更新Shader参数
	*/
	void flushParameters();

private:
	struct Buffer;
	void createBuffers(Buffer*& buffers, int buffer_size);
	void createBuffer(Buffer* buffer, int size);

private:
	struct Buffer
	{
		char *data;
		int size;
		ID3D11Buffer* buffer;

		//! 保存每个变量的Offset key:index value:offset
		std::map<int, int> offset;
	};

	ID3D11VertexShader *vertex_shader_;
	std::vector<Buffer*> vertex_buffers_;
	std::map<std::string, int> vertex_shader_parameter_index_;

	ID3D11PixelShader *fragment_shader_;
	std::vector<Buffer*> fragment_buffers_;
	std::map<std::string, int> fragment_shader_parameter_index_;
};

ANNA_NAMESPACE_END