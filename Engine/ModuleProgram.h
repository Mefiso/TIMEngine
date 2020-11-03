#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleProgram : public Module
{
public:
	unsigned int ID;
	ModuleProgram();
	~ModuleProgram();

	void CreateProgramFromFile(const char* vertexPath, const char* fragmentPath);

	bool CleanUp();
private:
	char* LoadShaderSource(const char* shader_file_name);
	unsigned CompileShader(unsigned type, const char* source);
	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader);
	void checkCompileErrors(unsigned int shader, const char* type);
};