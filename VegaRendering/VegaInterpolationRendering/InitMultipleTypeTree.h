#pragma once
#include <iostream>
#include <GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include<algorithm>
#include "../RenderingProcess/Camera.h"
#include "Shader.h"
#include "Sence.h"
#include "Mesh.h"
#include "VegaFemFactory.h"
#include "../Common/WindFiled.h"
#include "LoadWindAndTreeConfig.h"

class CInitMultipleTypeTree
{
public:
    CInitMultipleTypeTree()=default;
    CInitMultipleTypeTree(int vTreeTypeNumber, int vAllTreeNumbers,bool vWindFieldType=true) ;
    void InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex,int vTypeTreeRelatedFileNumber);
    void InitWindAndTree(int vTreeNumber, const std::string & vFilePath);
    void InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);
    void InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);

    void InitTreeModel(const std::string& vModelPath, int vTreeTypeIndex);
	int getSumFaceVerticesBeforeEndMesh(const int& vTreeIndex);
	void ComputeSumFaceVerticesBeforeEndMesh(int vTreeTypeNumber);
    //void InitASpecificWindSourceTreeRotation(int vTreeNumber, const std::string & vFilePath);
    //a common wind comming from a direction
    void InitMultipleExtraWindData(int vTreeTypeIndex);

    //a specific wind source
    void InitASpecificWindSourceWindData(int vTreeTypeIndex);
    void SetASpecificWindSourceTreeData();

    void InitFemFrameStruct(int vTreeTypeIndex);
    void InitShadowCubeMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT,  std::vector <glm::mat4>& vshadowTransforms,glm::vec3 * vlightVertices,glm::vec3 * vlightColors);

	void InitShadowMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT, glm::mat4& vshadowTransforms, glm::vec3 & vlightVertices, glm::vec3 & vlightColors);

    void InitScenceShaderData(int vTreeTypeIndex,float vScale);
    ~CInitMultipleTypeTree()=default;

    std::vector<CVegaFemFactory*> * getFemFactory() { return &m_MultipleTypeFem; };
    std::vector<CSence*> * getTreeModel() { return &m_MultipleTreeModel; };
    std::vector<std::vector<std::vector<int>>> * getExtraForces() { return &m_MultipleExtraForces; };
    std::vector<std::vector<std::vector<Common::SForceDirection>>> * getExtraDirection() { return &m_MultipleExtraDirections; };
    std::vector<std::vector<int>> *getTreesNumberSubjected2SameWind() { return &m_MultipleTreesNumberSubjected2SameWind; };
    std::vector<CShader*> * getScenceShadowShader() { return &m_MultipleSceneShadowShader; };
    std::vector<CShader*> * getScenceDepthShader() { return &m_MultipleSceneDepthShader; };
   

    CVegaFemFactory* getSpecificFemFactory(int vTreeTypeIndex) { return m_MultipleTypeFem[vTreeTypeIndex]; };
    CSence * getSpecificTreeModel(int vTreeTypeIndex) { return m_MultipleTreeModel[vTreeTypeIndex]; };
    std::vector<std::vector<int>> * getSpecificExtraForces(int vTreeTypeIndex) { return &m_MultipleExtraForces[vTreeTypeIndex]; };
    std::vector<std::vector<Common::SForceDirection>> * getSpecificExtraDirection(int vTreeTypeIndex) { return &m_MultipleExtraDirections[vTreeTypeIndex]; };
    std::vector<int> *getSpecificTreesNumberSubjected2SameWind(int vTreeTypeIndex) { return &m_MultipleTreesNumberSubjected2SameWind[vTreeTypeIndex]; };
    CShader * getSpecificScenceShadowShader(int vTreeTypeIndex) { return m_MultipleSceneShadowShader[vTreeTypeIndex]; };
    CShader * getSpecificScenceDepthShader(int vTreeTypeIndex) { return m_MultipleSceneDepthShader[vTreeTypeIndex]; };
    CLoadWindAndTreeConfig getSpecificLoadWindAndTree(int vTreeTypeIndex) { return m_MultipleTypeTree[vTreeTypeIndex];}

    std::vector<int> getTreeTypeIndex() { return m_TreeTypeIndex; };

	void updataTreeOnTerrain(int vTreeTypeIndex);
	void calculateTreeDistantWithTerrain(int vTreeTypeIndex);

	void setTerrainPara(std::vector<double> & vTerrainHeigth, double vTerrainX, double vTerrainZ, double XDensityScale, double ZDensityScale,double vTerrainHeightZDensity);
	
	void setTreeAxisYPara();
	void updataTreeOnTerrainByPara(int vTreeTypeIndex);
	//Rendering
	void UpdataSencePara(CCamera& vCamera, int vscrwidth, int vscrheight);
	void RenderingDepth(int vTreeTypeIndex, int vPlaneOrTree,float vTime,int vFrameIndex,int vWaveMap);
	void RenderingModel(int vTreeTypeIndex, unsigned int vdepthMap, int vPlaneOrTree,float vTime,int vFrameIndex,int vWaveMap,int vBendScale,int vPrimaryOffsetScale,bool vshadows);
	void Draw(CShader & vShader, CSence& vModel);

private:

    bool m_OneDirectionWindOrSpecificWindSource;

    void __GenerateTreesPosition();
    void __GenerateRandomTreesPosition() { m_AllTreesPosition = RandomTreePositionGenerate(m_AllTreesNumber); };
    void __GenerateStableTreesPosition() { m_AllTreesPosition = StableTreePositionGenerate(m_AllTreesNumber); };

    int m_TreeTypeNumber;
    //each type tree have one 
    std::vector<CVegaFemFactory*> m_MultipleTypeFem;
    std::vector<CLoadWindAndTreeConfig> m_MultipleTypeTree;
    std::vector<int> m_MultipleEachTreeRelatedFileNumber;
    std::vector<int> m_MultipleEachTreeProductNumber;
    std::vector<CShader*> m_MultipleSceneShadowShader;
    std::vector<CShader*> m_MultipleSceneDepthShader;
    std::vector<CSence *> m_MultipleTreeModel;

    //第一个std::vector表示某棵树，第二个vector表示该棵树共有多少中不同的形变，第三个vector表示某一形变收到的外力
    std::vector<std::vector<std::vector<int>>> m_MultipleExtraForces;
    std::vector<std::vector<std::vector<Common::SForceDirection>>> m_MultipleExtraDirections;
    std::vector<std::vector<int>> m_MultipleTreesNumberSubjected2SameWind;

    std::vector<int> m_TreeTypeIndex;

    //ShadowCubePara
    float m_vNearPlane;
    float m_vFarPlane;
    int m_ShadowWidth;
    int m_ShadowHeight;
	std::vector<int> m_SumFaceVerticesBeforeEndMesh;
    glm::vec3 m_LightVertice;
    glm::mat4 m_ShadowTransforms;

    glm::vec3 m_lightVertices;
    glm::vec3 m_lightColors;

    int m_AllTreesNumber;
    std::vector<int> m_EachTypeTreesPositonArray;
    std::vector<std::pair<double, double>> m_AllTreesPosition;

    std::vector<glm::vec3> m_AllTreesPositionInSence;

    std::vector<std::vector<glm::vec3>> m_EachTypeTreesPositionInSence;

    CWindField * m_ASpecificWindSource;

	//model
	std::vector<glm::mat4> m_ModelScale;
	std::vector<double> m_ModelScaleDouble;

	//Screen
	CCamera m_Camera;
	int m_SCRWidth;
	int m_SCRHeight;

	//Terrain
	std::vector<double> m_TerrainHeightY;
	std::vector<std::vector<double>> EachTypeGrassLocatedHeightofTerrain;
	std::vector<glm::mat4 *> m_InstanceObjectDumMat;

	//存在高度范围
	//std::vector<Mo>
	double m_TerrainX;
	double m_TerrainZ;

	double m_TerrainXDensityScale;
	double m_TerrainZDensityScale;

	double m_TerrainHeightZDensity;
	//handle
	std::vector<std::vector<double>> m_AxisYByhandle;
};
