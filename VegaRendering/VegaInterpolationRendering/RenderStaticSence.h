#pragma once
#include <iostream>
#include <GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include "../RenderingProcess/Camera.h"
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
		
		void loadStaticModel(std::string vmodelname,std::string vmodelFileName);
		void loadDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);
		void loadSenceShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath = nullptr);
		void setModelInstanceAndTransform(int vModelIndex, std::vector<std::pair<double, double>> vTransforms, std::vector<float> vRotations, int vInstanceNumber);
	
		void initModelShaderPara(int vModelIndex,float vTransForm);
		void RenderingDepth(int vModelIndex);
		void RenderingModel(int vModelIndex, unsigned int depthMap, bool vshadows);
		void Render(int vModelIndex);
		void Draw(CShader & vShader, CSence& vModel);
		void setSencePara(CCamera vCamera, int vscrwith, int vscrheight);

		void setTerrain(int vModelIndex, std::vector<double> vYTransFormations);

		void setGrassOnTerrain(int vGrassType);

		void getTerrain();

		void setTerrainHeightYToZero();
		
		void setTerrainHightMesh(double vXsize, double vZSize, double vXDensity, double vYDensity);

		void updataGrassOnTerrain(int vGrassType);

		void getTerrainPara(std::vector<double> & vTerrainHeigth, double& vTerrainX, double& vTerrainZ, double& XDensityScale, double& ZDensityScale, int& vTerrainHeightZDensity);

		double getAverage(int vZIndex, int vXIndex);
	private:

		void __getTerrainXZsize();
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
		//���ڸ߶ȷ�Χ
		double m_TerrainX;
		double m_TerrainZ;

		//x,z //��ǰ��ͼ��С-60��+60 x,z�ᶼ��
		std::pair<double, double> m_TerrainMaxSize;
		std::pair<double, double> m_TerrainMinSize;
		glm::mat4 m_InstanceMat;
		glm::mat4 m_TerrainMat;
		//X����������ĵ���
		int m_TerrainHeightXDensity;
		//Z���������ĵ���
		int m_TerrainHeightZDensity;
		

		double m_TerrainXDensityScale;
		double m_TerrainZDensityScale;

		//����һ������X*Z��������Ĵ�������ÿ�����ľ�����m_TerrainXDensityScale,����ÿ�����ʾ���뷶ΧΪm_TerrainXDensityScale
		
		std::vector<double> m_TerrainHeightY;

		std::vector<glm::mat4 *> m_InstanceObjectDumMat;

		//ÿ�����ͣ�ÿ�����͵�Instance
		std::vector<std::vector<double>> EachTypeGrassLocatedHeightofTerrain;
};