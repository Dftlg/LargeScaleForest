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

    void UpdataSSBOBindingPointIndex();

	void setSSBOUdeformationAndIndx4ShadowMapShader(const CShader& vShader);

	std::vector<std::vector<int>> getGroupsIndex() { return m_GroupsIndex; }
	std::vector<CMesh> getMeshes() { return m_Meshes; }
	void senceDraw(const CShader& vShader, std::vector<std::vector<glm::vec3>> deformationFrames);
	void setFileDirectiory(std::string vFileDirectory) { m_FileDirectory = vFileDirectory; }
	int getAssimpVerticesNumber() { return m_AssimpVerticesNumber; }
	void draw(const CShader& vShader);
	void setMeshRotation(std::vector<float> &vRotations, std::vector<std::pair<double, double>>& vTransFormations,float vScaleNumber,int vTreesNumber);
    //void setMeshRotation();

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
	/*void UpdataMeshTreeAndFrameIndex(std::vector<int>& vTreeFileIndex, std::vector<int>& vFrameIndex);*/
	bool gammaCorrection;

	glm::mat4 getInstanceMat() { return m_InstanceMat; };

	glm::mat4* getInstanceDumMat() { return m_InstanceDumMat; };



private:
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
	double *m_RestVertexs;
	std::vector<Common::STexture> m_Textures;
	std::string m_FileDirectory;
	std::string m_Data;
	unsigned int m_DataSize;
	std::vector<std::vector<glm::vec3>> m_EachFrameOfGroupData;
	//?????????group?????????????????????
	std::vector<std::vector<int>> m_GroupsIndex;

	CVegaFemFactory* m_VegaFactory;
	glm::vec4 * m_DeltaDeformationU;

	glm::vec4 * m_DeformationU;

	glm::ivec2 * m_TreeFileAndFrameIndex;

	unsigned int m_DeltaUSSBO;
	unsigned int m_UdeformationSSBO;
	unsigned int m_TreeFileAndFrameSSBO;

    std::vector<GLuint> m_SSBO_Binding_Point_Index;

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
};
