#pragma once
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Mesh.h"
//#include "TreeInstanceMesh.h"
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include "common.h"
#include "ExtraTool.h"
#include "VegaFemFactory.h"
#include "ObjStruct.h"
#include "objMesh.h"
#include "sceneObject.h"
#include "objMeshRender.h"
#include "ComputerShader.h"

class CSence
{
public:
	CSence() = default;
	CSence(const std::string& vModelPath, bool vGamma = false, bool vloadNormalizeModelAndFaceNormal = false) :gammaCorrection(vGamma) { __loadModel(vModelPath, vloadNormalizeModelAndFaceNormal); };
	CSence(const ObjMesh * vobjMesh, double * vRestVertexs) : m_Mesh(vobjMesh), m_RestVertexs(vRestVertexs) { __changeObjMeshStruct2Charptr(1); __loadModelFromMemory(); };
	CSence(const CMesh& vMesh);
	~CSence() = default;

	void Clear() { glDeleteBuffers(1, &m_DeltaUSSBO);   glDeleteBuffers(1, &m_UdeformationSSBO);};
	std::vector<std::vector<glm::vec3>>& getGroupDeformationData() { return m_EachFrameOfGroupData; };
	void SetParaMesh();
	void setGroupsIndex(CVegaFemFactory& vfemFactoryObject);
	void setVerticesNumber(CVegaFemFactory& vfemFactoryObject);
    void setTreeNumber(const int vInstanceTreeNumber);
	void setAssimpVerticesNumber();
	void initSSBODeformationDeltaU(CVegaFemFactory & vFem, int vFileNumber);
	void setSSBO4UDeformationAndIndex(CShader& vShader);
    void setSSBO4GenBufferUDeformationAndIndex(CShader& vShader,const int vTreeTypeIndex);
	void initSSBODeformationU();
	void initSSBOTreeFileAndFrameIndex(const int vTreeNumber);
	void resetSSBO4UDeformation();

	//Normla
	void initComputerSSBONormalRelatedData(ComputerShader& vShader, const int vTreeTypeIndex);
	void initSSBO4GenBufferNormal(CShader& vShader, const int vTreeTypeIndex);
	void setSSBO4GenModelNormalMatrixData(glm::mat4 vmodelMatrix);
	void initSSBO4GenModelNormalMatrixBuffer(CShader& vShader, const int vTreeTypeIndex);

    void UpdataSSBOBindingPointIndex();

	void setSSBOUdeformationAndIndx4ShadowMapShader(const CShader& vShader);

	std::vector<std::vector<int>> getGroupsIndex() { return m_GroupsIndex; }
	void getModelVertex();
	std::vector<CMesh> getMeshes() { return m_Meshes; }
	void senceDraw(const CShader& vShader, std::vector<std::vector<glm::vec3>> deformationFrames);
	void setFileDirectiory(std::string vFileDirectory) { m_FileDirectory = vFileDirectory; }
	int getAssimpVerticesNumber() { return m_AssimpVerticesNumber; }
	void draw(const CShader& vShader);
	void setMeshRotation(std::vector<float> &vRotations, std::vector<std::pair<double, double>>& vTransFormations,float vScaleNumber,int vTreesNumber);
    //void setMeshRotation();
    void setObjectPositionAndRotation(glm::vec3 vPosition,float vRotation);

	void setTerrainMesh(std::vector<double>& vZTransFormations);
    void setScaleMesh(float vScale, glm::mat4 * vmodelMatrices);
    void setMeshRotationRelationWindFieldAndTreeDirection(std::vector<float> &vRotations);
    void specificTreeRotation(std::vector<float> &vRotations, glm::mat4* vmodelMatrces);
    glm::mat4* translateTreePosition();
    void randomScaleMesh(glm::mat4 * vmodelMatrices);
	void randomRotation(glm::mat4* vmodelMatrices);
	void specificTreeZTransform(std::vector<double> &vYTransFormations, glm::mat4* vmodelMatrces);
	void setMeshGroupAndAssimpIndex();

	void UpdataYAxis(std::vector<double>& vYTransFormations);

    std::vector<glm::vec3> getTreePositions() { return m_TreePositions; };

    std::vector<Common::SForceDirection> getAngles() { return m_Angles; };

	void getModelHeight();

	void UpdataSSBOMeshTreeAndFrameIndex(std::vector<std::pair<int,int>>& vTreeFileAndFrameIndex);

	void ComputerShaderCalculateNormal(ComputerShader& vShader);
	/*void UpdataMeshTreeAndFrameIndex(std::vector<int>& vTreeFileIndex, std::vector<int>& vFrameIndex);*/
	bool gammaCorrection;

	glm::mat4* getInstanceMat() { return &m_InstanceMat; };

	glm::mat4* getInstanceDumMat() { return m_InstanceDumMat; };



private:
	void __setVertexRelatedFace();
	void __changeObjMeshStruct2Charptr(int vOutputMaterials=1);
	void __loadModel(const std::string& vModelPath, bool vloadNormalizeModel);
	void __loadModelFromMemory();
	void __processNode(const aiNode* vNode, const aiScene* vScene,bool vSaveDeformationOrLoadData);
	CMesh __processMesh(const aiMesh* vMesh, const aiScene* vScene);
	void __processSaveDeformation(const aiMesh*vMesh, const aiScene*vScene);
	std::vector<Common::STexture> loadMaterialTextures(aiMaterial *vMat, aiTextureType vType, std::string vtypeName);
	unsigned int TextureFromFile(const char *vPath, const std::string &vDirectory, bool vGamma = false);

	const ObjMesh * m_Mesh;
	std::vector<CMesh> m_Meshes;
	glm::vec3 * m_staticMeshVertex;

	double *m_RestVertexs;
	std::vector<Common::STexture> m_Textures;
	std::string m_FileDirectory;
	std::string m_Data;
	unsigned int m_DataSize;
	std::vector<std::vector<glm::vec3>> m_EachFrameOfGroupData;
	//所有的group中面的顶点索引
	std::vector<std::vector<int>> m_GroupsIndex;

	//前两个用来找第三个具体每个顶点相关的面
	//每个顶点与多少个面相连
	std::vector<int> m_EachVertexWithFaceNumber;
	//每个顶点与AllVertexRelatedFaceIndex中的最开头索引位置
	std::vector<int> m_EachVertexWithFaceFirstIndex;
	//存储连续的数组数组中每个值为一个顶点与周围那几个面相关的索引号
	std::vector<int> m_AllVertexRelatedFaceIndex;


	CVegaFemFactory* m_VegaFactory;
	glm::vec4 * m_DeltaDeformationU;

	glm::vec4 * m_DeformationU;

	glm::vec4 * m_Normal;

	glm::ivec2 * m_TreeFileAndFrameIndex;

	unsigned int m_DeltaUSSBO;
	unsigned int m_UdeformationSSBO;
	unsigned int m_TreeFileAndFrameSSBO;
	unsigned int m_NormalSSBO;
	unsigned int m_ModelNormalMatrixSSBO;

	//VertexRelatedFace
	unsigned int m_VertexWithFaceNumberSSBO;
	unsigned int m_VertexWithFaceFirstSSBO;
	unsigned int m_AllVertexRelatedFaceSSBO;

    std::vector<GLuint> m_SSBO_Binding_Point_Index;

	std::vector<GLuint> m_SSBO_Binding_Point_Index_Normal;

    std::vector<glm::vec3> m_TreePositions;

	int m_FrameNums;
	int m_VertexNums;
	int m_FileNumber;

	int m_VerticesNumber;

	int m_AssimpVerticesNumber;

    std::vector<float> m_SetRotation;
    std::vector<Common::SForceDirection> m_Angles;
    std::vector<std::pair<double, double>> m_TransFormations;
    int m_InstanceTreeNumber;

	std::vector<double> m_TerrainHeight;

	glm::mat4 m_InstanceMat;

	glm::mat4 *m_InstanceDumMat;

	glm::mat4 *m_InstanceNoramMatrix;
};
