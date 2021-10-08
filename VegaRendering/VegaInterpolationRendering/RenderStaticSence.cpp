#include "RenderStaticSence.h"

CRenderStaticSence::CRenderStaticSence(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT, glm::mat4& vshadowTransforms, glm::vec3 & vlightVertices, glm::vec3 & vlightColors)
{
	m_vNearPlane = vNearPlane;
	m_vFarPlane = vFarPlane;
	m_ShadowWidth = vSHADOW_WIDTH;
	m_ShadowHeight = vSHADOW_HEIGHT;
	m_LightVertice = vlightVertices;
	m_ShadowTransforms = vshadowTransforms;
	
	m_lightColors = vlightColors;
}

void CRenderStaticSence::loadStaticModel(std::string vmodelname, std::string vmodelFileName)
{
	m_ModelNames.push_back(vmodelname);
	CSence * ourModel = new CSence(vmodelFileName);
	m_Models.push_back(ourModel);
}

void CRenderStaticSence::loadDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
	CShader * ourDepthShader = new CShader(vVertexPath, vFragmentPath, vGeometryPath);
	m_DepthShaders.push_back(ourDepthShader);
}

void CRenderStaticSence::loadSenceShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
	CShader * ourModelShader = new CShader(vVertexPath, vFragmentPath, vGeometryPath);
	m_ModelShaders.push_back(ourModelShader);
}

//其他类型植被
void CRenderStaticSence::setModelInstanceAndTransform(int vModelIndex, std::vector<std::pair<double, double>> vTransforms, std::vector<float> vRotations,int vInstanceNumber)
{
	m_Models[vModelIndex]->setMeshRotation(vRotations, vTransforms,1,vInstanceNumber);
	m_InstanceObjectDumMat.push_back(m_Models[vModelIndex]->getInstanceDumMat());
	EachTypeObjectNumber.push_back(vInstanceNumber);
}

void CRenderStaticSence::setTerrain(int vModelIndex, std::vector<double> vYTransFormations)
{
	m_Models[vModelIndex]->setTerrainMesh(vYTransFormations);
	m_InstanceMat = m_Models[vModelIndex]->getInstanceMat();
}

void CRenderStaticSence::initModelShaderPara(int vModelIndex,float vTransForm)
{
	glm::mat4 model = glm::mat4(1.0f);

	//原版植被移动
	//model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.2f));



	model = glm::scale(model, glm::vec3(vTransForm, vTransForm, vTransForm));	// it's a bit too big for our scene, so scale it down

	m_ModelScale.push_back(model);
	m_ModelScaleDouble.push_back(vTransForm);
	m_DepthShaders[vModelIndex]->use();
	m_DepthShaders[vModelIndex]->setFloat("far_plane", m_vFarPlane);
	m_DepthShaders[vModelIndex]->setVec3("lightPos", m_LightVertice);
	/*for (unsigned int i = 0; i < 6; ++i)
	{
		m_DepthShaders[vModelIndex]->setMat4("lightSpaceMatrix", m_ShadowTransforms);
	}*/
	m_DepthShaders[vModelIndex]->setMat4("lightSpaceMatrix", m_ShadowTransforms);

	m_DepthShaders[vModelIndex]->setMat4("model", model);


	//set light to fragment
	m_ModelShaders[vModelIndex]->use();
	//for (unsigned int i = 0; i < sizeof(m_lightVertices) / sizeof(m_lightVertices[0]); ++i)
	//{
	//	glm::vec3 newPos = m_lightVertices[i];
	//	m_ModelShaders[vModelIndex]->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
	//	m_ModelShaders[vModelIndex]->setVec3("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);
	//}
	m_ModelShaders[vModelIndex]->setMat4("lightSpaceMatrix", m_ShadowTransforms);
	m_ModelShaders[vModelIndex]->setVec3("lightPositions", m_LightVertice);
	m_ModelShaders[vModelIndex]->setVec3("lightColors", m_lightColors);

	m_ModelShaders[vModelIndex]->setFloat("metallic", 0.04);
	m_ModelShaders[vModelIndex]->setFloat("roughness", 0.8);
	m_ModelShaders[vModelIndex]->setFloat("ao", 1.0f);
	m_ModelShaders[vModelIndex]->setInt("depthMap", 8);
	m_ModelShaders[vModelIndex]->setFloat("far_plane", m_vFarPlane);	
	m_ModelShaders[vModelIndex]->setMat4("model", model);

}

void CRenderStaticSence::RenderingDepth(int vModelIndex)
{
	m_DepthShaders[vModelIndex]->use();

	Draw(*m_DepthShaders[vModelIndex], *m_Models[vModelIndex]);
	//glm::mat4 projection = glm::perspective(glm::radians(vCamera.getZoom()), (float)vscrwith / (float)vscrheight, 0.1f, 100.0f);
	//glm::mat4 view = vCamera.getViewMatrix();
	//m_DepthShaders[vModelIndex]->setMat4("projection", projection);
	//m_DepthShaders[vModelIndex]->setMat4("view", view);
	//m_DepthShaders[vModelIndex]->setVec3("camPos", vCamera.getPosition());
	//glm::mat4 model = glm::mat4(1.0f);
	////model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));// translate it down so it's at the center of the scene
	////model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));
	//model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.2f));
	//model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
	//m_DepthShaders[vModelIndex]->setMat4("model", model);
	//m_Models[vModelIndex]->draw(*m_DepthShaders[vModelIndex]);

}

void CRenderStaticSence::RenderingModel(int vModelIndex, unsigned int depthMap,bool vshadows)
{
	m_ModelShaders[vModelIndex]->use();
	m_ModelShaders[vModelIndex]->setInt("shadows", vshadows);
	glActiveTexture(GL_TEXTURE8);
    auto temp = glGetUniformLocation(m_ModelShaders[vModelIndex]->getID(), "depthMap");
    glUniform1i(temp, 8);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	Draw(*m_ModelShaders[vModelIndex], *m_Models[vModelIndex]);
}

//void CRenderStaticSence::RenderingModel(int vModelIndex, unsigned int depthCubeMap, bool vshadows)
//{
//	m_ModelShaders[vModelIndex]->use();
//	m_ModelShaders[vModelIndex]->setInt("shadows", vshadows);
//}
//
//void CRenderStaticSence::Render(int vModelIndex)
//{
//	Draw(*m_ModelShaders[vModelIndex], *m_Models[vModelIndex]);
//}

void CRenderStaticSence::Draw(CShader & vShader, CSence& vModel)
{
	vShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.getZoom()), (float)SCRWidth / (float)SCRHeight, 0.1f, 100.0f);
	glm::mat4 view = m_Camera.getViewMatrix();
	vShader.setMat4("projection", projection);
	vShader.setMat4("view", view);
	vShader.setVec3("camPos", m_Camera.getPosition());
	
	vModel.draw(vShader);
}

void CRenderStaticSence::setSencePara(CCamera vCamera, int vscrwidth, int vscrheight)
{
	m_Camera = vCamera;
	SCRWidth = vscrwidth;
	SCRHeight = vscrheight;
}

void CRenderStaticSence::setTerrainHeightYToZero()
{
	m_TerrainHeightY.resize((m_TerrainHeightXDensity * 2)*(m_TerrainHeightZDensity * 2),0);
}

void CRenderStaticSence::getTerrain()
{
//对于没有的值取周围值平均一下	
	m_TerrainXDensityScale = m_TerrainX / m_TerrainHeightXDensity;
	m_TerrainZDensityScale = m_TerrainZ / m_TerrainHeightZDensity;
	m_TerrainHeightY.resize((m_TerrainHeightXDensity*2)*(m_TerrainHeightZDensity*2),-10);

	m_TerrainMat = m_ModelScale[0] * m_InstanceMat;
	std::vector<CMesh> tempMesh = m_Models[0]->getMeshes();
	int tempXIndex;
	int tempZIndex;
	for (int i = 0; i < tempMesh.size(); i++)
	{
		std::vector<Common::SVertex> tempVectors = tempMesh[i].getVertices();
		for (int k = 0; k < tempVectors.size(); k++)
		{
			//获取转换后的大小

			glm::vec4 tempPosition = m_TerrainMat * glm::vec4(tempVectors[k].Position, 1.0);
			if (tempPosition.x > (-m_TerrainX) && tempPosition.x < m_TerrainX&&tempPosition.z>(-m_TerrainZ)&& tempPosition.z< m_TerrainZ)
			{
				tempXIndex = (tempPosition.x - (-m_TerrainX)) / m_TerrainXDensityScale;
				tempZIndex= (tempPosition.z - (-m_TerrainZ)) / m_TerrainZDensityScale;
				m_TerrainHeightY[tempZIndex*(m_TerrainHeightZDensity * 2) + tempXIndex] = tempPosition.y;
			}

		}
		//m_TerrainSize
	}
	//将为99的值用周围邻近的值取平均
	int doubleterrainHeightXDensity = m_TerrainHeightXDensity * 2;
	int doubleterrainHeightZDensity = m_TerrainHeightZDensity * 2;
	for (int i = 1; i < doubleterrainHeightZDensity-1; i++)
	{
		for (int k = 1; k < doubleterrainHeightXDensity-1; k++)
		{
			if (m_TerrainHeightY[doubleterrainHeightXDensity*i + k] == -10)
			{	
				//std::cout << "i="<<i << "k=" << k << std::endl;
				m_TerrainHeightY[doubleterrainHeightXDensity*i + k]=getAverage(i, k);
			}
		}
	}

}

double CRenderStaticSence::getAverage(int vZIndex, int vXIndex)
{
	int doubleterrainHeightXDensity = m_TerrainHeightXDensity * 2;
	int doubleterrainHeightZDensity = m_TerrainHeightZDensity * 2;

	int existHeightY = 0;
	double sumHeightY = 0;

	if (m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex-1) + vXIndex - 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex - 1) + vXIndex - 1];

	/*	std::cout << "0-0" << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex - 1) + vXIndex - 1] << std::endl;*/
	}

	if (m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex-1) + vXIndex] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex - 1) + vXIndex];
	/*	std::cout << "0-1" << std::endl;
		std::cout << doubleterrainHeightXDensity * (vZIndex - 1) << std::endl;
		std::cout << vXIndex << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex - 1) + vXIndex] << std::endl;*/
	}

	if (m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex-1) + vXIndex + 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex - 1) + vXIndex + 1];
	/*	std::cout << "0-2" << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex - 1) + vXIndex + 1] << std::endl;*/
	}

	if (m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex - 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex - 1];
		/*std::cout << "0-3" << std::endl;
		std::cout << m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex - 1] << std::endl;*/
	}

	if (m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex + 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex + 1];
		/*std::cout << "0-4" << std::endl;
		std::cout << m_TerrainHeightY[(doubleterrainHeightXDensity)*vZIndex + vXIndex + 1] << std::endl;*/
	}
	
	if (m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex+1) + vXIndex - 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex + 1) + vXIndex - 1];
	   /* std::cout << "0-5" << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex + 1) + vXIndex - 1] << std::endl;*/
	}


	if (m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex+1) + vXIndex] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex + 1) + vXIndex];
		/*std::cout << "0-6" << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity*(vZIndex + 1) + vXIndex] << std::endl;*/
	}

	if (m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex+1) + vXIndex + 1] != -10)
	{
		existHeightY += 1;
		sumHeightY += m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex + 1) + vXIndex + 1];
		/*std::cout << "0-7" << std::endl;
		std::cout << m_TerrainHeightY[doubleterrainHeightXDensity *(vZIndex + 1) + vXIndex + 1] << std::endl;*/
	}
	/*std::cout << sumHeightY << std::endl;
	std::cout << existHeightY << std::endl;*/
	return sumHeightY / existHeightY;
}

void CRenderStaticSence::setTerrainHightMesh(double vXsize, double vZSize, double vXDensity,double vZDensity)
{
	__getTerrainXZsize();
	m_TerrainX = vXsize;
	m_TerrainZ = vZSize;

	m_TerrainX = m_TerrainMaxSize.first;
	m_TerrainZ = m_TerrainMaxSize.second;

	m_TerrainHeightXDensity = vXDensity;
	m_TerrainHeightZDensity = vZDensity;
}
//
void CRenderStaticSence::__getTerrainXZsize()
{
	m_TerrainMat = m_ModelScale[0] * m_InstanceMat;
	std::vector<CMesh> tempMesh = m_Models[0]->getMeshes();
	//int size
	m_TerrainMaxSize = std::make_pair(0, 0);
	m_TerrainMinSize = std::make_pair(0, 0);

	for (int i = 0; i < tempMesh.size(); i++)
	{
		std::vector<Common::SVertex> tempVectors = tempMesh[i].getVertices();	
		for (int k = 0; k < tempVectors.size(); k++)
		{
			glm::vec4 tempPosition= m_TerrainMat*glm::vec4(tempVectors[k].Position,1.0);
			if (tempPosition.x > m_TerrainMaxSize.first)
			{
				m_TerrainMaxSize.first = tempPosition.x;
			}
			if (tempPosition.x < m_TerrainMinSize.first)
			{
				m_TerrainMinSize.first = tempPosition.x;
			}

			if (tempPosition.z > m_TerrainMaxSize.second)
			{
				m_TerrainMaxSize.second = tempPosition.z;
			}
			if (tempPosition.z < m_TerrainMinSize.second)
			{
				m_TerrainMinSize.second = tempPosition.z;
			}
		}
		//m_TerrainSize
	}

}

void CRenderStaticSence::setGrassOnTerrain(int vGrassType)
{
	//获取该植被对应的向上两种旋转矩阵，Instance和model

	std::vector<double> ModelLocatedHeightofTerrain(EachTypeObjectNumber[vGrassType - 1]);

	glm::mat4* TypeGrassDumInstanceMat= m_InstanceObjectDumMat[vGrassType -1];

	int tempXIndex, tempZIndex;
	//该类型每棵草的旋转矩阵
	for (int i = 0; i < EachTypeObjectNumber[vGrassType - 1]; i++)
	{
		glm::mat4 tempMat = m_ModelScale[vGrassType] * TypeGrassDumInstanceMat[i];

		std::vector<CMesh> tempMesh = m_Models[vGrassType]->getMeshes();

		//获取模型中一点
		std::vector<Common::SVertex> tempVectors = tempMesh[0].getVertices();
		//该模型点转换到世界坐标系
		glm::vec4 tempPosition = tempMat * glm::vec4(tempVectors[0].Position, 1.0);

		//获取在该地面的位置的高度
		tempXIndex = (tempPosition.x - (-m_TerrainX)) / m_TerrainXDensityScale;
		tempZIndex = (tempPosition.z - (-m_TerrainZ)) / m_TerrainZDensityScale;
		std::cout << tempXIndex << std::endl;
		std::cout << tempZIndex << std::endl;
		ModelLocatedHeightofTerrain[i] = m_TerrainHeightY[tempZIndex*(m_TerrainHeightZDensity * 2) + tempXIndex];
		ModelLocatedHeightofTerrain[i] = ModelLocatedHeightofTerrain[i] / m_ModelScaleDouble[vGrassType];
		//ModelLocatedHeightofTerrain[i] = 0;
	}	
	//获取在世界坐标系下的Y相对值，但是因为最后要乘以model矩阵导致尺度缩小，因此需要再乘一次scale先是使其放大。

	EachTypeGrassLocatedHeightofTerrain.push_back(ModelLocatedHeightofTerrain);
}

void CRenderStaticSence::updataGrassOnTerrain(int vGrassType)
{
	m_Models[vGrassType]->UpdataYAxis(EachTypeGrassLocatedHeightofTerrain[vGrassType-1]);
}

void CRenderStaticSence::getTerrainPara(std::vector<double> & vTerrainHeigth, double& vTerrainX, double& vTerrainZ, double& XDensityScale, double& ZDensityScale, int& vTerrainHeightZDensity)
{
	vTerrainHeigth = m_TerrainHeightY;
	vTerrainX = m_TerrainX;
	vTerrainZ = m_TerrainZ;
	XDensityScale = m_TerrainXDensityScale;
	ZDensityScale = m_TerrainZDensityScale;
	vTerrainHeightZDensity = m_TerrainHeightZDensity;
}
