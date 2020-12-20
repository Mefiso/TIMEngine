#pragma once
#include "Globals.h"
#include "GL/glew.h"
#include <Math/float2.h>
#include <Math/float3.h>
#include <Math/float4.h>
#include <Math/float3x3.h>
#include <Math/float4x4.h>

namespace ModuleProgram
{
	unsigned int CreateProgramFromFile(const char* vertexPath, const char* fragmentPath);	// Creates a Shading Program from the path of the Vertex and Fragment shader files

	namespace
	{
		// Check for shader compilation/linking errors.
		void checkCompileErrors(unsigned int shader, const char* type)
		{
			int success;
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					int len = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
					if (len > 0)
					{
						int written = 0;
						char* infoLog = (char*)malloc(len);
						glGetShaderInfoLog(shader, len, &written, infoLog);
						LOG("[error] ERROR::SHADER_COMPILATION_ERROR of type: %s \n%s\n -- --------------------------------------------------- -- ", type, infoLog);
						free(infoLog);
					}
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					int len = 0;
					glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);
					if (len > 0)
					{
						int written = 0;
						char* infoLog = (char*)malloc(len);
						glGetProgramInfoLog(shader, len, &written, infoLog);
						LOG("[error] ERROR::PROGRAM_LINKING_ERROR of type: %s \n%s\n -- --------------------------------------------------- -- ", type, infoLog);
						free(infoLog);
					}
				}
			}
		}

		// Read a file and store the text in a char* buffer
		char* LoadShaderSource(const char* shader_file_name)
		{
			char* data = nullptr;
			FILE* file = nullptr;
			fopen_s(&file, shader_file_name, "rb");
			if (file)
			{
				fseek(file, 0, SEEK_END);
				int size = ftell(file);
				data = (char*)malloc(size + 1);
				fseek(file, 0, SEEK_SET);
				fread(data, 1, size, file);
				data[size] = 0;
				fclose(file);
			}
			else
				LOG("[error] Can't read file %s", shader_file_name);
			return data;
		}

		// Compile a shader written in GLSL, from the text stored in a char* buffer
		unsigned int CompileShader(unsigned int type, const char* source)
		{
			unsigned shader_id = glCreateShader(type);
			glShaderSource(shader_id, 1, &source, 0);
			glCompileShader(shader_id);
			const char* type_name = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
			checkCompileErrors(shader_id, type_name);

			return shader_id;
		}

		// Generate a shading program from the compiled Vertex and Fragment shaders
		unsigned int CreateProgram(unsigned int vtx_shader, unsigned int frg_shader)
		{
			unsigned program_id = glCreateProgram();
			glAttachShader(program_id, vtx_shader);
			glAttachShader(program_id, frg_shader);
			glLinkProgram(program_id);
			checkCompileErrors(program_id, "PROGRAM");
			glDeleteShader(vtx_shader);
			glDeleteShader(frg_shader);
			return program_id;
		}
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use(unsigned int ID);
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(unsigned int ID, const std::string name, bool value);
	// ------------------------------------------------------------------------
	void setInt(unsigned int ID, const std::string name, int value);
	// ------------------------------------------------------------------------
	void setFloat(unsigned int ID, const std::string name, float value);
	// ------------------------------------------------------------------------
	void setVec2(unsigned int ID, const char* name, const float2& value);
	void setVec2(unsigned int ID, const std::string& name, float x, float y);
	// ------------------------------------------------------------------------
	void setVec3(unsigned int ID, const std::string& name, const float3& value);
	void setVec3(unsigned int ID, const std::string& name, float x, float y, float z);
	// ------------------------------------------------------------------------
	void setVec4(unsigned int ID, const std::string& name, const float4& value);
	void setVec4(unsigned int ID, const std::string& name, float x, float y, float z, float w);
	// ------------------------------------------------------------------------
	void setMat3(unsigned int ID, const std::string& name, const float3x3& mat);
	// ------------------------------------------------------------------------
	void setMat4(unsigned int ID, const std::string& name, const float4x4& mat);
};