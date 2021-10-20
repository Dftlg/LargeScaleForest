#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ComputerShader.h"

ComputerShader::ComputerShader(const char* vComputerPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string ComputerShaderCode;
	std::ifstream ComputerShaderFile;
	// ensure ifstream objects can throw exceptions:
	ComputerShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		ComputerShaderFile.open(vComputerPath);
		std::stringstream ComputerShaderStream;
		ComputerShaderStream << ComputerShaderFile.rdbuf();
		ComputerShaderFile.close();
		ComputerShaderCode = ComputerShaderStream.str();
	}
	catch (std::ifstream::failure Error)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* ComputerCode = ComputerShaderCode.c_str();
	// 2. compile shaders
	unsigned int Computer;
	// vertex shader
	Computer = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(Computer, 1, &ComputerCode, NULL);
	glCompileShader(Computer);
	__checkCompileErrors(Computer, "Computer");
	// shader Program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, Computer);
	glLinkProgram(m_ID);
	__checkCompileErrors(m_ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(Computer);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::use()
{
	glUseProgram(m_ID);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setBool(const std::string& vName, bool vValue) const
{
	glUniform1i(glGetUniformLocation(m_ID, vName.c_str()), (int)vValue);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setInt(const std::string& vName, int vValue) const
{
	glUniform1i(glGetUniformLocation(m_ID, vName.c_str()), vValue);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setFloat(const std::string& vName, float vValue) const
{
	glUniform1f(glGetUniformLocation(m_ID, vName.c_str()), vValue);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec2(const std::string& vName, const glm::vec2& vValue) const
{
	glUniform2fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec2(const std::string& vName, float vX, float vY) const
{
	glUniform2f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec3(const std::string& vName, const glm::vec3& vValue) const
{
	glUniform3fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec3(const std::string& vName, float vX, float vY, float vZ) const
{
	glUniform3f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY, vZ);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec4(const std::string& vName, const glm::vec4& vValue) const
{
	glUniform4fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setVec4(const std::string& vName, float vX, float vY, float vZ, float vW)
{
	glUniform4f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY, vZ, vW);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setMat2(const std::string& vName, const glm::mat2& vMat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setMat3(const std::string& vName, const glm::mat3& vMat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::setMat4(const std::string& vName, const glm::mat4& vMat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void ComputerShader::__checkCompileErrors(GLuint vShader, std::string vType)
{
	GLint Success;
	GLchar InfoLog[1024];
	if (vType != "PROGRAM")
	{
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &Success);
		if (!Success)
		{
			glGetShaderInfoLog(vShader, 1024, NULL, InfoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << vType << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(vShader, GL_LINK_STATUS, &Success);
		if (!Success)
		{
			glGetProgramInfoLog(vShader, 1024, NULL, InfoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << vType << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}