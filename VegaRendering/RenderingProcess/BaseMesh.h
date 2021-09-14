#pragma once
#include "common.h"
#include "Shader.h"
#include <string>

class CBaseMesh
{
public:
	CBaseMesh() = default;
	virtual ~CBaseMesh() = default;

	unsigned int getVAO() const { return m_VAO; }
	virtual void draw(const CShader& vShader) const = 0;

protected:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;

private:
	virtual void __setupMesh() = 0;

};