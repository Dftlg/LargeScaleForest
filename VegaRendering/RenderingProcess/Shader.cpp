#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

CShader::CShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath, const char* vTesControlPath, const char* vTesEvaluationPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string VertexCode;
	std::string FragmentCode;
	std::string GeometryCode;
	std::string TessellationControlCode;
	std::string TessellationEvaluationCode;
	std::ifstream VertexShaderFile;
	std::ifstream FragmentShaderFile;
	std::ifstream GeometryShaderFile;
	std::ifstream TescShaderFile;
	std::ifstream TeseShaderFile;
	// ensure ifstream objects can throw exceptions:
	VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	GeometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	TescShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	TeseShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		VertexShaderFile.open(vVertexPath);
		FragmentShaderFile.open(vFragmentPath);
		std::stringstream VertexShaderStream, FragmentShaderStream;
		// read file's buffer contents into streams
		VertexShaderStream << VertexShaderFile.rdbuf();
		FragmentShaderStream << FragmentShaderFile.rdbuf();
		// close file handlers
		VertexShaderFile.close();
		FragmentShaderFile.close();
		// convert stream into string
		VertexCode = VertexShaderStream.str();
		FragmentCode = FragmentShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (vGeometryPath != nullptr)
		{
			GeometryShaderFile.open(vGeometryPath);
			std::stringstream GeometryShaderStream;
			GeometryShaderStream << GeometryShaderFile.rdbuf();
			GeometryShaderFile.close();
			GeometryCode = GeometryShaderStream.str();
		}
		if (vTesControlPath != nullptr)
		{
			TescShaderFile.open(vTesControlPath);
			std::stringstream TescShaderStream;
			TescShaderStream << TescShaderFile.rdbuf();
			TescShaderFile.close();
			TessellationControlCode = TescShaderStream.str();
		}
		if (vTesEvaluationPath != nullptr)
		{
			TeseShaderFile.open(vTesEvaluationPath);
			std::stringstream TeseShaderStream;
			TeseShaderStream << TeseShaderFile.rdbuf();
			TeseShaderFile.close();
			TessellationEvaluationCode = TeseShaderStream.str();
		}
	}
	catch (std::ifstream::failure Error)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* VertexShaderCode = VertexCode.c_str();
	const char* FragmentShaderCode = FragmentCode.c_str();
	// 2. compile shaders
	unsigned int Vertex, Fragment;
	// vertex shader
	Vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vertex, 1, &VertexShaderCode, NULL);
	glCompileShader(Vertex);
	__checkCompileErrors(Vertex, "VERTEX");
	// fragment Shader
	Fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fragment, 1, &FragmentShaderCode, NULL);
	glCompileShader(Fragment);
	__checkCompileErrors(Fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int Geometry, TessellationControl, TessellationEvaluation;
	if (vGeometryPath != nullptr)
	{
		const char* GeometryShaderCode = GeometryCode.c_str();
		Geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(Geometry, 1, &GeometryShaderCode, NULL);
		glCompileShader(Geometry);
		__checkCompileErrors(Geometry, "GEOMETRY");
	}
	if (vTesControlPath != nullptr)
	{
		const char * TescShaderCode = TessellationControlCode.c_str();
		TessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(TessellationControl, 1, &TescShaderCode, NULL);
		glCompileShader(TessellationControl);
		__checkCompileErrors(TessellationControl, "TESSELLATION CONTROL");
	}
	if (vTesEvaluationPath != nullptr)
	{
		const char * TeseShaderCode = TessellationEvaluationCode.c_str();
		TessellationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(TessellationEvaluation, 1, &TeseShaderCode, NULL);
		glCompileShader(TessellationEvaluation);
		__checkCompileErrors(TessellationEvaluation, "TESSELLATION CONTROL");
	}
	// shader Program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, Vertex);
	glAttachShader(m_ID, Fragment);
	if (vGeometryPath != nullptr)
		glAttachShader(m_ID, Geometry);
	if (vTesControlPath != nullptr)
		glAttachShader(m_ID, TessellationControl);
	if (vTesEvaluationPath != nullptr)
		glAttachShader(m_ID, TessellationEvaluation);
	glLinkProgram(m_ID);
	__checkCompileErrors(m_ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(Vertex);
	glDeleteShader(Fragment);
	if (vGeometryPath != nullptr)
		glDeleteShader(Geometry);
}

//****************************************************************************************************
//FUNCTION:
void CShader::use()
{
	glUseProgram(m_ID);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setBool(const std::string& vName, bool vValue) const
{
	glUniform1i(glGetUniformLocation(m_ID, vName.c_str()), (int)vValue);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setInt(const std::string& vName, int vValue) const
{
	glUniform1i(glGetUniformLocation(m_ID, vName.c_str()), vValue);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setFloat(const std::string& vName, float vValue) const
{
	glUniform1f(glGetUniformLocation(m_ID, vName.c_str()), vValue);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec2(const std::string& vName, const glm::vec2& vValue) const
{
	glUniform2fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec2(const std::string& vName, float vX, float vY) const
{
	glUniform2f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec3(const std::string& vName, const glm::vec3& vValue) const
{
	glUniform3fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec3(const std::string& vName, float vX, float vY, float vZ) const
{
	glUniform3f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY, vZ);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec4(const std::string& vName, const glm::vec4& vValue) const
{
	glUniform4fv(glGetUniformLocation(m_ID, vName.c_str()), 1, &vValue[0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setVec4(const std::string& vName, float vX, float vY, float vZ, float vW)
{
	glUniform4f(glGetUniformLocation(m_ID, vName.c_str()), vX, vY, vZ, vW);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setMat2(const std::string& vName, const glm::mat2& vMat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setMat3(const std::string& vName, const glm::mat3& vMat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::setMat4(const std::string& vName, const glm::mat4& vMat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, vName.c_str()), 1, GL_FALSE, &vMat[0][0]);
}

//****************************************************************************************************
//FUNCTION:
void CShader::__checkCompileErrors(GLuint vShader, std::string vType)
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