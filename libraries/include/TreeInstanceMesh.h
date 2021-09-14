#pragma once
#include "BaseMesh.h"
#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "common.h"
#include "Sence.h"

class CTreeInstanceMesh 
{
public:
	CTreeInstanceMesh() = default;
	CTreeInstanceMesh(CSence vModel);
	~CTreeInstanceMesh() = default;

	void __setupMesh();
	void draw(const CShader& vShader);

private:
	CSence m_Model;
	unsigned int *m_VBOGroupIndex;
	glm::mat4* m_modelMatrices;
	std::vector<std::vector<int>> m_GroupsIndex;
};

#pragma once
