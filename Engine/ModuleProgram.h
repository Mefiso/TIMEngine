#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include <Math/float2.h>
#include <Math/float3.h>
#include <Math/float4.h>
#include <Math/float3x3.h>
#include <Math/float4x4.h>

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	unsigned int CreateProgramFromFile(const char* vertexPath, const char* fragmentPath) const;

	bool CleanUp() override;

private:
	char* LoadShaderSource(const char* shader_file_name) const;
	unsigned CompileShader(unsigned type, const char* source) const;
	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader) const;
	void checkCompileErrors(unsigned int shader, const char* type) const;
public:
    // activate the shader
    // ------------------------------------------------------------------------
    void use(unsigned int ID)
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(unsigned int ID, const std::string name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(unsigned int ID, const std::string name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(unsigned int ID, const std::string name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(unsigned int ID, const char* name, const float2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name), 1, &value.x);
    }
    void setVec2(unsigned int ID, const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(unsigned int ID, const std::string& name, const float3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
    }
    void setVec3(unsigned int ID, const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(unsigned int ID, const std::string& name, const float4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value.x);
    }
    void setVec4(unsigned int ID, const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat3(unsigned int ID, const std::string& name, const float3x3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, (const float*) &mat);
    }
    // ------------------------------------------------------------------------
    void setMat4(unsigned int ID, const std::string& name, const float4x4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_TRUE, (const float*) &mat);
    }
};