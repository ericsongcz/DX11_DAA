#include "D3D11Shader.h"
#include "../Math/Math.h"

ANNA_NAMESPACE_BEGIN

D3D11Shader::D3D11Shader()
: vertex_shader_(nullptr),
fragment_shader_(nullptr)
{

}

D3D11Shader::~D3D11Shader()
{

}

void D3D11Shader::compile()
{

}

bool D3D11Shader::compile(const std::string& file, const char* entry, const char* model, ID3DBlob** buffer)
{
	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* error_buffer = nullptr;

	HRESULT result = D3DX11CompileFromFile(D3D11Helper::stringToLPCWSTR(file), nullptr, nullptr, entry, model, flags, 0, nullptr, buffer, &error_buffer, nullptr);

	if (D3D11Helper::checkError(result))
	{
		if (error_buffer != nullptr)
		{
			Log::error(static_cast<char*>(error_buffer->GetBufferPointer()));
			error_buffer->Release();
		}

		return false;
	}

	if (error_buffer != nullptr)
		error_buffer->Release();

	return true;
}

bool D3D11Shader::loadVertex(const std::string& file)
{
	ID3D11Device *pDevice = D3D11Helper::getDevice();

	ID3DBlob* pBuffer = nullptr;

	bool result = compile(file, "main", D3D11Helper::getVertexProfile(), &pBuffer);

	if (!result)
	{
		Log::error("D3D11Shader::loadVertex %s failed", file.c_str());
		return false;
	}

	HRESULT d3d_result = pDevice->CreateVertexShader(pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), nullptr, &vertex_shader_);

	if (D3D11Helper::checkError(d3d_result))
	{
		Log::error("D3D11Shader::loadVertex CreateVertexShader %s failed", file.c_str());

		if (pBuffer != nullptr)
		{
			pBuffer->Release();
			pBuffer = nullptr;
		}

		return false;
	}

	//设定参数

	//设定参数名称到索引的映射


	return true;
}

bool D3D11Shader::loadFragment(const std::string& file)
{
	ID3D11Device *pDevice = D3D11Helper::getDevice();

	ID3DBlob* pBuffer = nullptr;

	bool result = compile(file, "main", D3D11Helper::getFragmentProfile(), &pBuffer);

	if (!result)
	{
		Log::error("D3D11Shader::loadFragment %s failed", file.c_str());
		return false;
	}

	HRESULT d3d_result = pDevice->CreatePixelShader(pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), nullptr, &fragment_shader_);

	if (D3D11Helper::checkError(d3d_result))
	{
		Log::error("D3D11Shader::loadFragment CreatePixelShader %s failed", file.c_str());

		if (pBuffer != nullptr)
		{
			pBuffer->Release();
			pBuffer = nullptr;
		}

		return false;
	}

	return true;
}

void D3D11Shader::bind_impl()
{
	flushParameters();

	ID3D11DeviceContext *context = D3D11Helper::getContext();
	ID3D11Buffer *d3d11_buffers[128];

	if (vertex_shader_ != nullptr)
	{
		context->VSSetShader(vertex_shader_, nullptr, 0);

		size_t buffer_size = vertex_buffers_.size();

		int i = 0;
		for (auto itr = vertex_buffers_.begin(); itr != vertex_buffers_.end(); ++itr)
		{
			Buffer* buffer = *itr;
			d3d11_buffers[i] = buffer->buffer;

			++i;
		}

		context->VSSetConstantBuffers(0, buffer_size, d3d11_buffers);
	}

	if (fragment_shader_ != nullptr)
	{
		context->PSSetShader(fragment_shader_, nullptr, 0);

		size_t buffer_size = fragment_buffers_.size();

		int i = 0;
		for (auto itr = fragment_buffers_.begin(); itr != fragment_buffers_.end(); ++itr)
		{
			Buffer* buffer = *itr;
			d3d11_buffers[i] = buffer->buffer;

			++i;
		}

		context->PSSetConstantBuffers(0, buffer_size, d3d11_buffers);
	}
}

void D3D11Shader::unbind_impl()
{
	ID3D11DeviceContext *context = D3D11Helper::getContext();

	if (vertex_shader_ != nullptr)
	{
		context->VSSetShader(nullptr, nullptr, 0);
	}

	if (fragment_shader_ != nullptr)
	{
		context->PSSetShader(nullptr, nullptr, 0);
	}
}

int D3D11Shader::findVertexParameter(const std::string& name)
{
	auto itr = vertex_shader_parameter_index_.find(name);

	if (itr != vertex_shader_parameter_index_.end())
	{
		return itr->second;
	}

	return -1;
}

int D3D11Shader::findFragmentParameter(const std::string& name)
{
	auto itr = fragment_shader_parameter_index_.find(name);

	if (itr != fragment_shader_parameter_index_.end())
	{
		return itr->second;
	}

	return -1;
}

void D3D11Shader::setVertexParameterByFloat(int id, const float* data, int offset, int size)
{
	setParameter(VERTEX, id, (const int*)data, offset, size);
}

void D3D11Shader::setVertexParameterByInt(int id, const int* data, int offset, int size)
{
	setParameter(VERTEX, id, data, offset, size);
}

void D3D11Shader::setFragmentParameterByFloat(int id, const float* data, int offset, int size)
{
	setParameter(FRAGMENT, id, (const int*)data, offset, size);
}

void D3D11Shader::setFragmentParameterByInt(int id, const int* data, int offset, int size)
{
	setParameter(FRAGMENT, id, data, offset, size);
}

void D3D11Shader::setParameter(ShaderType type, int id, const int* data, int offset, int size)
{
	Buffer* target_buffer = nullptr;

	if (type == VERTEX)
	{
		if (static_cast<size_t>(id) >= vertex_buffers_.size())
			return;

		target_buffer = vertex_buffers_[id];
	}
	else if (type == FRAGMENT)
	{
		if (static_cast<size_t>(id) >= fragment_buffers_.size())
			return;

		target_buffer = fragment_buffers_[id];
	}

	if (target_buffer == nullptr)
	{
		return;
	}

	if (target_buffer->data == nullptr)
	{
		Log::error("D3D11Shader::setParameter refBuffer.data is null");
		return;
	}

	if (Math::memcmp(target_buffer->data + offset, data, size))
	{
		Math::memcpy(target_buffer->data + offset, data, size);
	}
}

void D3D11Shader::createBuffers(Buffer*& buffers, int buffer_size)
{
	buffers = new Buffer[buffer_size];
	Math::memset(buffers, 0, sizeof(Buffer)* buffer_size);
}

void D3D11Shader::createBuffer(Buffer* buffer, int size)
{
	ID3D11Device *pDevice = D3D11Helper::getDevice();

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = size;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	if (D3D11Helper::checkError(pDevice->CreateBuffer(&bufferDesc, nullptr, &buffer->buffer)) == true)
	{
		Log::error("D3D11Shader::createBuffer failed");
	}
}

void D3D11Shader::flushParameters()
{
	ID3D11DeviceContext *context = D3D11Helper::getContext();

	for (auto itr = vertex_buffers_.begin(); itr != vertex_buffers_.end(); ++itr)
	{
		Buffer *buffer = *itr;

		if (buffer->buffer == nullptr)
		{
			createBuffer(buffer, buffer->size);
		}

		context->UpdateSubresource(buffer->buffer, 0, nullptr, buffer->data, 0, 0);
	}
	
	for (auto itr = fragment_buffers_.begin(); itr != fragment_buffers_.end(); ++itr)
	{
		Buffer *buffer = *itr;

		if (buffer->buffer == nullptr)
		{
			createBuffer(buffer, buffer->size);
		}

		context->UpdateSubresource(buffer->buffer, 0, nullptr, buffer->data, 0, 0);
	}
}

void D3D11Shader::addParameter(ShaderType type, const std::string& name, int parameter_size)
{
	Buffer* target_buffer = nullptr;

	if (type == VERTEX)
	{
		int index = vertex_shader_parameter_index_.size();
		vertex_shader_parameter_index_.insert(std::make_pair(name, index));

		Buffer* new_buffer = new Buffer;
		Math::memset(new_buffer, 0, sizeof(Buffer));
		vertex_buffers_.push_back(new_buffer);

		target_buffer = new_buffer;
	}
	else if (type == FRAGMENT)
	{
		int index = fragment_shader_parameter_index_.size();
		fragment_shader_parameter_index_.insert(std::make_pair(name, index));

		Buffer* new_buffer = new Buffer;
		Math::memset(new_buffer, 0, sizeof(Buffer));
		fragment_buffers_.push_back(new_buffer);

		target_buffer = new_buffer;
	}

	target_buffer->data = new char[parameter_size];
	target_buffer->size = parameter_size;

	if (target_buffer->buffer == nullptr)
	{
		createBuffer(target_buffer, parameter_size);
	}
}

ANNA_NAMESPACE_END