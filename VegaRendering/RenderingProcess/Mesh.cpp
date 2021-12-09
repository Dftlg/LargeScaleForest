#include "Mesh.h"

CMesh::CMesh(const std::vector<Common::SVertex>& vVertices, const std::vector<unsigned int>& vIndices, const std::vector<Common::STexture> vTestures)
{
	__setParameter(vVertices, vIndices, vTestures);
	__setupMesh();
}

//****************************************************************************************************
//FUNCTION:
void CMesh::__setParameter(const std::vector<Common::SVertex>& vVertices, const std::vector<unsigned int>& vIndices, const std::vector<Common::STexture> vTestures)
{
	_ASSERT(vVertices.size() > 0);

	this->m_Vertices = vVertices;
	this->m_Indices = vIndices;
	this->m_Textures = vTestures;
}

//****************************************************************************************************
//FUNCTION:
void CMesh::reset(const std::vector<Common::SVertex>& vVertices, const std::vector<unsigned int>& vIndices, const std::vector<Common::STexture> vTestures)
{
	__setParameter(vVertices, vIndices, vTestures);
	__setupMesh();
}

void CMesh::setRotation(glm::mat4 * temp,int vInstanceTreeNumber)
{
	m_modelMatrices = temp;
    m_InstanceTreeNumber = vInstanceTreeNumber;
	__setupInstanceMesh();
}

//void CMesh::setSpecificRotation(glm::mat4 *temp)
//{
//    m_specificmodelRotationMatrices = temp;
//    __setupInstanceSpecificRotationMesh();
//}

void CMesh::setGroupAndAssimpIndex(std::vector<int>& vGroupIndex, int vLastCapacity, int vNextCapacity)
{
	m_GroupIndex = vGroupIndex;
	for (int i = 0; i < vNextCapacity; i++)
	{
		m_AssimpGroupIndex.push_back(i + vLastCapacity);
	}
	__setupGroupIndexMesh();
	__setupAssimpGroupIndex();
}

//****************************************************************************************************
//必有instance导致每次重复了很多instance旋转矩阵
void CMesh::draw(const CShader& vShader) const
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int OpacityNr = 1;
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string Number;
		std::string Name = m_Textures[i].type;
		if (Name == "texture_diffuse")
			Number = std::to_string(diffuseNr++);
		else if (Name == "texture_specular")
			Number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (Name == "texture_normal")
			Number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (Name == "texture_opacity")
			Number = std::to_string(OpacityNr++); // transfer unsigned int to stream

												 // now set the sampler to the correct texture unit

		auto temp = glGetUniformLocation(vShader.getID(), (Name + Number).c_str());
		glUniform1i(temp, i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
	}

	// draw mesh
	glBindVertexArray(m_VAO);
	//glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
    //glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

//****************************************************************************************************
void CMesh::drawLine(const CShader& vShader) const
{

    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}

//****************************************************************************************************
//FUNCTION:
void CMesh::__setupMesh()
{ 
	// create buffers/arrays
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	//m_Mesh.
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Common::SVertex), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Common::SVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Common::SVertex), (void*)offsetof(Common::SVertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Common::SVertex), (void*)offsetof(Common::SVertex, TexCoords));
	if (Common::ModelHaveTangentAndBitangent)
	{
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Common::SVertex), (void*)offsetof(Common::SVertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Common::SVertex), (void*)offsetof(Common::SVertex, Bitangent));
	}
	glBindVertexArray(0);
}

void CMesh::__setupInstanceMesh()
{
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, m_InstanceTreeNumber * sizeof(glm::mat4), &m_modelMatrices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);

}
//void CMesh::__setupInstanceSpecificRotationMesh()
//{
//    glBindVertexArray(m_VAO);
//
//    glGenBuffers(1, &m_InstanceSpecificRotationVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceSpecificRotationVBO);
//    glBufferData(GL_ARRAY_BUFFER, Common::TreesNumber * sizeof(glm::mat4), &m_specificmodelRotationMatrices[0], GL_STATIC_DRAW);
//
//    glEnableVertexAttribArray(9);
//    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
//    glEnableVertexAttribArray(10);
//    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
//    glEnableVertexAttribArray(11);
//    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
//    glEnableVertexAttribArray(12);
//    glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));
//
//    glVertexAttribDivisor(9, 1);
//    glVertexAttribDivisor(10, 1);
//    glVertexAttribDivisor(11, 1);
//    glVertexAttribDivisor(12, 1);
//
//    glBindVertexArray(0);
//}

void CMesh::__setupGroupIndexMesh()
{
	glBindVertexArray(m_VAO);
	
	glGenBuffers(1, &m_GroupIndexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_GroupIndexVBO);
	glBufferData(GL_ARRAY_BUFFER, m_GroupIndex.size() * sizeof(int), &(m_GroupIndex[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(int), (void*)0);
	glBindVertexArray(0);

}

void CMesh::__setupAssimpGroupIndex()
{
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_AssimpGroupIndexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_AssimpGroupIndexVBO);
	glBufferData(GL_ARRAY_BUFFER, m_AssimpGroupIndex.size() * sizeof(int), &(m_AssimpGroupIndex[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(8);
	glVertexAttribIPointer(8, 1, GL_INT, sizeof(int), (void*)0);
	glBindVertexArray(0);
}


