#include "ModuleProgram.h"
#include "Leaks.h"

ModuleProgram::ModuleProgram()
{
}

// Destructor
ModuleProgram::~ModuleProgram()
{
}

unsigned int ModuleProgram::CreateProgramFromFile(const char* vertexPath, const char* fragmentPath) const
{
	const char* vShaderCode = LoadShaderSource(vertexPath);
	const char* fShaderCode = LoadShaderSource(fragmentPath);
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode);
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode);
	unsigned int ID = CreateProgram(vertex, fragment);
	free((void*) vShaderCode);
	free((void*) fShaderCode);

	return ID;
}

// Called before quitting
bool ModuleProgram::CleanUp()
{
	LOG("Destroying Shader Module");

	return true;
}

char* ModuleProgram::LoadShaderSource(const char* shader_file_name) const
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

unsigned ModuleProgram::CompileShader(unsigned type, const char* source) const
{
	unsigned shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);
	glCompileShader(shader_id);
	const char* type_name = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
	checkCompileErrors(shader_id, type_name);

	return shader_id;
}

unsigned ModuleProgram::CreateProgram(unsigned vtx_shader, unsigned frg_shader) const
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

// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
void ModuleProgram::checkCompileErrors(unsigned int shader, const char* type) const
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