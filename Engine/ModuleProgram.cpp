#include "ModuleProgram.h"
#include "Leaks.h"

unsigned int ModuleProgram::CreateProgramFromFile(const char* vertexPath, const char* fragmentPath)
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

// activate the shader
    // ------------------------------------------------------------------------
void ModuleProgram::use(unsigned int ID)
{
    glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void ModuleProgram::setBool(unsigned int ID, const std::string name, bool value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void ModuleProgram::setInt(unsigned int ID, const std::string name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ModuleProgram::setFloat(unsigned int ID, const std::string name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void ModuleProgram::setVec2(unsigned int ID, const char* name, const float2& value)
{
    glUniform2fv(glGetUniformLocation(ID, name), 1, &value.x);
}
void ModuleProgram::setVec2(unsigned int ID, const std::string& name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void ModuleProgram::setVec3(unsigned int ID, const std::string& name, const float3& value)
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
}
void ModuleProgram::setVec3(unsigned int ID, const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void ModuleProgram::setVec4(unsigned int ID, const std::string& name, const float4& value)
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
}
void ModuleProgram::setVec4(unsigned int ID, const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ModuleProgram::setMat3(unsigned int ID, const std::string& name, const float3x3& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, (const float*)&mat);
}
// ------------------------------------------------------------------------
void ModuleProgram::setMat4(unsigned int ID, const std::string& name, const float4x4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, (const float*)&mat);
}