#pragma once
#include <iostream>
#include <GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "../RenderingProcess/Camera.h"
#include "../Common/common.h"
#include "Shader.h"
#include "Sence.h"
#include "Mesh.h"
#include <stdio.h>
#include <stdlib.h> 

class CRenderStaticSence
{
	public:
		CRenderStaticSence() = default;
		CRenderStaticSence(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT, glm::mat4& vshadowTransforms, glm::vec3 & vlightVertices, glm::vec3 & vlightColors);
        ~CRenderStaticSence();
		void loadStaticModel(std::string vmodelname,std::string vmodelFileName);
		void loadDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);
		void loadSenceShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);
		void setModelInstanceAndTransform(std::string vModelName, std::vector<std::pair<double, double>> vTransforms, std::vector<float> vRotations, int vInstanceNumber);

        void setObjectTransform(std::string vModelName, glm::vec3 vPosition, float vRotation);
        void setObjectTransform(std::string vModelName, glm::mat4 & vMat);
		void initModelShaderPara(std::string vModelName);
        void initModelShaderVegPara(std::string vModelName, glm::mat4& TreeMat);
		void RenderingDepth(std::string vModelName);
		void RenderingModel(std::string vModelName, unsigned int depthMap, bool vshadows);
        void RenderingModelWithWireframe(std::string vModelName, bool resetModelMatrix, glm::mat4& vModelMatrix);
		void Render(int vModelIndex);
		void Draw(CShader & vShader, CSence& vModel, Common::DrawType vType);
		void setSencePara(CCamera vCamera, int vscrwith, int vscrheight);

		void setTerrain(std::string vModelName, std::vector<double> vYTransFormations);

		void setGrassOnTerrain(std::string vGrassName);

		void getTerrain(std::string vModelName);

		void setTerrainHeightYToZero();
		
		void setTerrainHightMesh(std::string vModelName,double vXsize, double vZSize, double vXDensity, double vYDensity);

		void updataGrassOnTerrain(std::string vGrassName,int vGrassIndexInallGrasses);

		void getTerrainPara(std::vector<double> & vTerrainHeigth, double& vTerrainX, double& vTerrainZ, double& XDensityScale, double& ZDensityScale, int& vTerrainHeightZDensity);

		double getAverage(int vZIndex, int vXIndex);

        void setModelScale(float vscale) { m_Modelscales.push_back(vscale); };

        void setVegMeshFragmentShaderRenderingColor(std::string vModelName,glm::vec4 vColor);

        int getObjectIndexByNameMap(std::string vObjectName) { return m_ModelNameAndIndexMap[vObjectName]; };
	private:

		void __getTerrainXZsize(std::string vModelName);
		std::vector<std::string> m_ModelNames;
		std::vector<CSence*> m_Models;
		std::vector<CShader*> m_DepthShaders;
		std::vector<CShader*> m_ModelShaders;

		std::vector<int> EachTypeObjectNumber;

		CCamera m_Camera;
		int SCRWidth;
		int SCRHeight;

		float m_vNearPlane;
		float m_vFarPlane;
		int m_ShadowWidth;
		int m_ShadowHeight;
		std::vector<int> m_SumFaceVerticesBeforeEndMesh;
		glm::vec3 m_LightVertice;
		glm::mat4 m_ShadowTransforms;

		glm::vec3 m_lightVertices;
		glm::vec3 m_lightColors;

		std::vector<glm::mat4> m_ModelScale;
		std::vector<double> m_ModelScaleDouble;
		//存在高度范围
		double m_TerrainX;
		double m_TerrainZ;

		//x,z //当前地图大小-60到+60 x,z轴都是
		std::pair<double, double> m_TerrainMaxSize;
		std::pair<double, double> m_TerrainMinSize;
		glm::mat4 m_InstanceMat;
		glm::mat4 m_TerrainMat;
		//X轴半轴的网格的点数
		int m_TerrainHeightXDensity;
		//Z轴半轴网格的点数
		int m_TerrainHeightZDensity;
		

		double m_TerrainXDensityScale;
		double m_TerrainZDensityScale;

		//建立一个含有X*Z网格点数的大型网格，每个结点的距离是m_TerrainXDensityScale,里面每个点表示距离范围为m_TerrainXDensityScale
		
		std::vector<double> m_TerrainHeightY;

		std::vector<glm::mat4 *> m_InstanceObjectDumMat;

		//每个类型，每个类型的Instance
		std::vector<std::vector<double>> EachTypeGrassLocatedHeightofTerrain;

        std::map<std::string, int> m_ModelNameAndIndexMap;

        std::vector<float> m_Modelscales;
};
