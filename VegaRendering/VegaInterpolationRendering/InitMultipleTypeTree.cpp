#include "InitMultipleTypeTree.h"

CInitMultipleTypeTree::CInitMultipleTypeTree(int vTreeTypeNumber, int vAllTreeNumbers, bool vWindFieldType)
{
    m_TreeTypeNumber = vTreeTypeNumber;
    m_AllTreesNumber = vAllTreeNumbers;
    m_MultipleExtraForces.resize(vTreeTypeNumber);
    m_MultipleExtraDirections.resize(vTreeTypeNumber);
    m_EachTypeTreesPositionInSence.resize(vTreeTypeNumber);
    m_OneDirectionWindOrSpecificWindSource = vWindFieldType;
    __GenerateTreesPosition();
    m_EachTypeTreesPositonArray.push_back(0);
    int Sum = 0;
    for (int i = 0; i < vTreeTypeNumber; i++)
    {
        Sum += Common::TreesNumbers[i];
        m_EachTypeTreesPositonArray.push_back(Sum);
    }
}

void CInitMultipleTypeTree::__GenerateTreesPosition()
{
	//__GenerateStableTreesPosition();
	//__GenerateRandomTreesPosition();
    if (m_OneDirectionWindOrSpecificWindSource == true)
        __GenerateRandomTreesPosition();
    else
        __GenerateStableTreesPosition();
}

void CInitMultipleTypeTree::InitVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex, int vTypeTreeRelatedFileNumber)
{
    CVegaFemFactory* vFem=new CVegaFemFactory(vDirectoryName, vMutilVerticesBaseFile, vCorrectDeformationUVertexIndex);
    std::vector<Common::SFileFrames> vtemp = vFem->searchFileFrameOnAttribute();
    m_MultipleEachTreeRelatedFileNumber.push_back(vTypeTreeRelatedFileNumber);
    for (int i = 0; i < vTypeTreeRelatedFileNumber; i++)
    {
        std::vector<Common::SFileFrames> temp;
        temp.push_back(vtemp[i]);
        vFem->readFramesDeformationData(temp, i);
    }
    m_MultipleTypeFem.push_back(vFem);
}

//在这里写位置
void CInitMultipleTypeTree::InitWindAndTree(int vTreeNumber, const std::string & vFilePath)
{
    m_MultipleEachTreeProductNumber.push_back(vTreeNumber);

    CLoadWindAndTreeConfig windAndTreeConfig(vTreeNumber, vFilePath);
    m_MultipleTypeTree.push_back(windAndTreeConfig);
}

//void CInitMultipleTypeTree::InitASpecificWindSourceTreeRotation(int vTreeNumber, const std::string & vFilePath)
//{
//    if (m_OneDirectionWindOrSpecificWindSource = true)
//        std::cout << "This step use on A Specific Wind Source,The other function InitWindAndTree use on a one Direction wind" << std::endl;
//    else
//    {
//        m_MultipleEachTreeProductNumber.push_back(vTreeNumber);
//
//    }
//}

void CInitMultipleTypeTree::InitSceneShadowShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
    CShader * ourSceneShadowShader=new CShader (vVertexPath, vFragmentPath, vGeometryPath);
    m_MultipleSceneShadowShader.push_back(ourSceneShadowShader);
}
void CInitMultipleTypeTree::InitSceneDepthShader(const char* vVertexPath, const char* vFragmentPath, const char* vGeometryPath)
{
    CShader * ourSceneDepthShader=new CShader(vVertexPath, vFragmentPath, vGeometryPath);
    m_MultipleSceneDepthShader.push_back(ourSceneDepthShader);
}

void CInitMultipleTypeTree::InitComputerShaderCalculateNormal(const char* vCompShader)
{
	ComputerShader * Ourcomputershader = new ComputerShader(vCompShader);
	m_MultipleCompCalculateNormalShader.push_back(Ourcomputershader);
}

void CInitMultipleTypeTree::setTreeModelMatrixToShader(int vTreeTypeIndex)
{
	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]->use();
	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]->setMat4("instanceMatrix", m_ModelScale[0]* m_InstanceObjectDumMat[0][0]);
	
}

void CInitMultipleTypeTree::InitTreeModel(const std::string& vModelPath,int vTreeTypeIndex)
{
    CSence* ourModel=new CSence(vModelPath);

    ourModel->setTreeNumber(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
    std::vector<float> SpecificRotation = m_MultipleTypeTree[vTreeTypeIndex].getMultipleRotationAngle();
    std::vector<std::pair<double, double>> tempTransFormation;
    for (int i = m_EachTypeTreesPositonArray[vTreeTypeIndex]; i < m_EachTypeTreesPositonArray[vTreeTypeIndex + 1]; i++)
        tempTransFormation.push_back(m_AllTreesPosition[i]);
    

    //set instance Matrix
    ourModel->setMeshRotation(SpecificRotation, tempTransFormation,1,Common::TreesNumbers[vTreeTypeIndex]);
	//get ModelInstance
	m_InstanceObjectDumMat.push_back(ourModel->getInstanceDumMat());

    m_EachTypeTreesPositionInSence[vTreeTypeIndex] = ourModel->getTreePositions();
    m_AllTreesPositionInSence.insert(m_AllTreesPositionInSence.end(), m_EachTypeTreesPositionInSence[vTreeTypeIndex].begin(), m_EachTypeTreesPositionInSence[vTreeTypeIndex].end());


    ourModel->setGroupsIndex(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel->setVerticesNumber(*(m_MultipleTypeFem[vTreeTypeIndex]));
    ourModel->setMeshGroupAndAssimpIndex();
   
    ourModel->initSSBODeformationDeltaU(*(m_MultipleTypeFem[vTreeTypeIndex]), m_MultipleEachTreeRelatedFileNumber[vTreeTypeIndex]);
    ourModel->initSSBODeformationU();
    ourModel->initSSBOTreeFileAndFrameIndex(m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
    ourModel->setSSBO4GenBufferUDeformationAndIndex(*(m_MultipleSceneShadowShader[vTreeTypeIndex]), vTreeTypeIndex);

	
    //ourModel->setSSBO4UDeformationAndIndex(*(m_MultipleSceneShadowShader[vTreeTypeIndex]));
    //ourModel->setSSBOUdeformationAndIndx4ShadowMapShader(*(m_MultipleSceneDepthShader[vTreeTypeIndex]));
    m_TreeTypeIndex.push_back(vTreeTypeIndex);
    m_MultipleTreeModel.push_back(ourModel);
}

int CInitMultipleTypeTree::getSumFaceVerticesBeforeEndMesh(const int & vTreeIndex)
{
	/*int sumFaceVerticesBeforeEndMesh = 0;
	for (int i = 0; i < m_MultipleTreeModel[vTreeIndex]->getMeshes().size() - 1; i++)
	{
		sumFaceVerticesBeforeEndMesh += m_MultipleTreeModel[vTreeIndex]->getMeshes()[i].getVertices().size();
	}*/
	return m_SumFaceVerticesBeforeEndMesh[vTreeIndex];
}

void CInitMultipleTypeTree::ComputeSumFaceVerticesBeforeEndMesh(int vTreeTypeNumber)
{
	int sumFaceVerticesBeforeEndMesh = 0;
	for (int i = 0; i < vTreeTypeNumber; i++)
	{
		for (int j = 0; j < m_MultipleTreeModel[i]->getMeshes().size() - 1; j++)
		{
			sumFaceVerticesBeforeEndMesh += m_MultipleTreeModel[i]->getMeshes()[i].getVertices().size();
		}
		m_SumFaceVerticesBeforeEndMesh.push_back(sumFaceVerticesBeforeEndMesh);
	}
}

//计算定向风场
void CInitMultipleTypeTree::InitMultipleExtraWindData(int vTreeTypeIndex)
{
    std::vector<std::vector<SWaveFunctionPara>> OneDirectionWindRelatedMultipleTree = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeWindPara();
    std::vector<int> TreesNumberSubjected2SameWind = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeDuplicateNumber();
    m_MultipleTreesNumberSubjected2SameWind.push_back(TreesNumberSubjected2SameWind);
    //只计算了树受到不同的风力，若相同则直接取用已有数据
    for (int i = 0; i < OneDirectionWindRelatedMultipleTree.size(); i++)
    {
        CWindField OnedirectionWind(Common::ProductFrameNumber, OneDirectionWindRelatedMultipleTree[i], 60000, Common::SForceDirection(0, 0), m_MultipleTypeTree[vTreeTypeIndex].getTreeRotationAngle()[i]);
        m_MultipleExtraForces[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindForces());
        m_MultipleExtraDirections[vTreeTypeIndex].push_back(OnedirectionWind.getDirectionWindDirection());
    }
}

void CInitMultipleTypeTree::InitASpecificWindSourceWindData(int vTreeTypeIndex)
{
    std::vector<int> TreesNumberSubjected2SameWind = m_MultipleTypeTree[vTreeTypeIndex].getMultipleTreeDuplicateNumber();
    m_MultipleTreesNumberSubjected2SameWind.push_back(TreesNumberSubjected2SameWind);
    //还没设置
    std::vector<std::vector<SWaveFunctionPara>> SpecificeWindFunctionPara;
    std::vector<glm::vec3> WindCenterMovePosition;
    std::vector<int> WindCenterMoveFrames;
    std::vector<double> MoveScale;
    m_ASpecificWindSource=new CWindField (glm::vec3(0, 0, 0), Common::ProductFrameNumber, SpecificeWindFunctionPara[0], 60000, 10, WindCenterMovePosition, WindCenterMoveFrames, MoveScale);
}

void CInitMultipleTypeTree::InitFemFrameStruct(int vTreeTypeIndex)
{
    m_MultipleTypeFem[vTreeTypeIndex]->initMatchedFrameStruct(m_MultipleExtraForces[vTreeTypeIndex].size());
    m_MultipleTypeFem[vTreeTypeIndex]->initKVFDataSearchRangeError();
}

//void CInitMultipleTypeTree::InitShadowCubeMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT, std::vector <glm::mat4>& vshadowTransforms, glm::vec3 * vlightVertices, glm::vec3 * vlightColors)
//{
//    m_vNearPlane= vNearPlane;
//    m_vFarPlane= vFarPlane;
//    m_ShadowWidth= vSHADOW_WIDTH;
//    m_ShadowHeight= vSHADOW_HEIGHT;
//    m_LightVertice= vlightVertices[0];
//    m_ShadowTransforms= vshadowTransforms;
//    for (int i = 0; i < 4; i++)
//    {
//        m_lightVertices[i] = vlightVertices[i];
//        m_lightColors[i] = vlightColors[i];
//    }
//
//}

void CInitMultipleTypeTree::InitShadowMapPara(float vNearPlane, float vFarPlane, int vSHADOW_WIDTH, int vSHADOW_HEIGHT,glm::mat4& vshadowTransforms, glm::vec3 & vlightVertices, glm::vec3 & vlightColors)
{
	m_vNearPlane = vNearPlane;
	m_vFarPlane = vFarPlane;
	m_ShadowWidth = vSHADOW_WIDTH;
	m_ShadowHeight = vSHADOW_HEIGHT;
	m_LightVertice = vlightVertices;
	m_ShadowTransforms = vshadowTransforms;

	m_lightColors = vlightColors;
	//for (int i = 0; i < 4; i++)
	//{
	//	m_lightVertices[i] = vlightVertices[i];
	//	m_lightColors[i] = vlightColors[i];
	//}

}

void CInitMultipleTypeTree::InitScenceShaderData(int vTreeTypeIndex,float vScale)
{
	//模型缩小在instanceMat里
	glm::mat4 model = glm::mat4(1.0f);
	/*model = glm::translate(model, glm::vec3(0.0f, -0.25f, -1.2f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));*/	// it's a bit too big for our scene, so scale it down
	//model = glm::scale(model, glm::vec3(vScale, vScale, vScale));

	model = glm::translate(model, glm::vec3(0.0f, -0.25f, -1.2f));
	model = glm::scale(model, glm::vec3(vScale, vScale, vScale));	// it's a bit too big for our scene, so scale it down
	m_ModelScale.push_back(model);


	m_ModelScaleDouble.push_back(vScale);

    //帧数
    int frameNums = m_MultipleTypeFem[vTreeTypeIndex]->getFileFrames(0)->Frames.size();
    //obj model vertices
    int vertexNums = m_MultipleTypeFem[vTreeTypeIndex]->getFileFrames(0)->Frames[0].BaseFileDeformations.size();
    std::cout <<"vertexNums"<< vertexNums << std::endl;
    m_MultipleSceneDepthShader[vTreeTypeIndex]->use();
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("frameNums", frameNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("vertexNums", vertexNums);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber());
    std::cout << "SceneDepth AssimpVerticesvertexNums" << m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber() << std::endl;

    m_MultipleSceneShadowShader[vTreeTypeIndex]->use();
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("frameNums", frameNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("vertexNums", vertexNums);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber());
    std::cout << "Scene AssimpVerticesvertexNums" << m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber() << std::endl;

    //create depth cubemap transformation matrices and some value
    m_MultipleSceneDepthShader[vTreeTypeIndex]->use();
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setFloat("far_plane", m_vFarPlane);
    m_MultipleSceneDepthShader[vTreeTypeIndex]->setVec3("lightPos", m_LightVertice);

	m_MultipleSceneDepthShader[vTreeTypeIndex]->setMat4("lightSpaceMatrix", m_ShadowTransforms);

	m_MultipleSceneDepthShader[vTreeTypeIndex]->setMat4("model", model);

    //set light to fragment
    m_MultipleSceneShadowShader[vTreeTypeIndex]->use();
    //for (unsigned int i = 0; i < sizeof(m_lightVertices) / sizeof(m_lightVertices[0]); ++i)
    //{
    //    glm::vec3 newPos = m_lightVertices[i];
    //    m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
    //    m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);
    //}
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightPositions", m_LightVertice);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setVec3("lightColors", m_lightColors);

    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("metallic", 0.04);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("roughness", 0.8);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("ao", 1.0f);
    //m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("depthMap", 8);
    m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("far_plane", m_vFarPlane);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setMat4("lightSpaceMatrix", m_ShadowTransforms);

	m_MultipleSceneShadowShader[vTreeTypeIndex]->setMat4("model", model);

	//m_MultipleSceneShadowShader[vTreeTypeIndex]->setBool("useGeomOrCompCalNormal", Common::UseGeomOrCompCalculateNormal);
}

//void CInitMultipleTypeTree::InitNormalRelatedDate(int vTreeTypeIndex)
//{
//	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]
//}

void CInitMultipleTypeTree::InitScenceNormalMatrixData(int vTreeTypeIndex)
{
	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]->use();
	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]->setInt("TreeNumber", m_MultipleEachTreeProductNumber[vTreeTypeIndex]);
	m_MultipleCompCalculateNormalShader[vTreeTypeIndex]->setInt("assimpvertexNums", m_MultipleTreeModel[vTreeTypeIndex]->getAssimpVerticesNumber());
	m_MultipleTreeModel[vTreeTypeIndex]->setSSBO4GenModelNormalMatrixData(m_ModelScale[vTreeTypeIndex]);
	m_MultipleTreeModel[vTreeTypeIndex]->initSSBO4GenModelNormalMatrixBuffer(*(m_MultipleCompCalculateNormalShader[vTreeTypeIndex]), vTreeTypeIndex);
	m_MultipleTreeModel[vTreeTypeIndex]->initComputerSSBONormalRelatedData(*(m_MultipleCompCalculateNormalShader[vTreeTypeIndex]), vTreeTypeIndex);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void CInitMultipleTypeTree::calculateTreeDistantWithTerrain(int vTreeTypeIndex)
{
	std::vector<double> TypeTreeLocatedHeightofTerrain(Common::TreesNumbers[vTreeTypeIndex]);
	glm::mat4* TypeGrassDumInstanceMat = m_InstanceObjectDumMat[vTreeTypeIndex];
	int tempXIndex, tempZIndex;

	for (int i = 0; i < Common::TreesNumbers[vTreeTypeIndex]; i++)
	{
		glm::mat4 tempMat = m_ModelScale[vTreeTypeIndex] * TypeGrassDumInstanceMat[i];

		std::vector<CMesh> tempMesh = m_MultipleTreeModel[vTreeTypeIndex]->getMeshes();

		//获取模型中一点
		std::vector<Common::SVertex> tempVectors = tempMesh[0].getVertices();
		//该模型点转换到世界坐标系
		glm::vec4 tempPosition = tempMat * glm::vec4(tempVectors[0].Position, 1.0);

		//获取在该地面的位置的高度
		tempXIndex = (tempPosition.x - (-m_TerrainX)) / m_TerrainXDensityScale;
		tempZIndex = (tempPosition.z - (-m_TerrainZ)) / m_TerrainZDensityScale;
		/*if (tempXIndex > m_TerrainX * 2 || tempZIndex > m_TerrainZ * 2)
		{
			TypeTreeLocatedHeightofTerrain[i] = 0;
			continue;
		}*/
		/*std::cout << tempXIndex << std::endl;
		std::cout << tempZIndex << std::endl;*/
		TypeTreeLocatedHeightofTerrain[i] = m_TerrainHeightY[tempZIndex*(m_TerrainHeightZDensity * 2) + tempXIndex];
		TypeTreeLocatedHeightofTerrain[i] = TypeTreeLocatedHeightofTerrain[i] / m_ModelScaleDouble[vTreeTypeIndex];
		//ModelLocatedHeightofTerrain[i] = 0;
	}
	for (int i = 0; i < Common::TreesNumbers[vTreeTypeIndex]; i++)
	{
		if (TypeTreeLocatedHeightofTerrain[i] > Common::ModelCutInTerrain[vTreeTypeIndex])
			TypeTreeLocatedHeightofTerrain[i] = -20;
	}
	EachTypeGrassLocatedHeightofTerrain.push_back(TypeTreeLocatedHeightofTerrain);
}

void CInitMultipleTypeTree::updataTreeOnTerrain(int vTreeTypeIndex)
{
	m_MultipleTreeModel[vTreeTypeIndex]->UpdataYAxis(EachTypeGrassLocatedHeightofTerrain[vTreeTypeIndex]);
}

void CInitMultipleTypeTree::updataTreeOnTerrainByPara(int vTreeTypeIndex)
{

	m_MultipleTreeModel[vTreeTypeIndex]->UpdataYAxis(m_AxisYByhandle[vTreeTypeIndex]);
}

void CInitMultipleTypeTree::setTreeAxisYPara()
{
	for (int i = 0; i < m_MultipleTreeModel.size(); i++)
	{
		std::vector<double> tempAxisY(Common::TreesNumbers[i], Common::ModelAxisYMove[i]);
		m_AxisYByhandle.push_back(tempAxisY);
	}
	
}

void CInitMultipleTypeTree::UpdataSencePara(CCamera& vCamera, int vscrwidth, int vscrheight)
{
	m_Camera = vCamera;
	m_SCRWidth = vscrwidth;
	m_SCRHeight = vscrwidth;
}

//用外面的draw就没问问题用里面的有问题....
void CInitMultipleTypeTree::RenderingDepth(int vTreeTypeIndex, int vPlaneOrTree, float vTime, int vFrameIndex, int vWaveMap)
{
	m_MultipleTreeModel[vTreeTypeIndex]->UpdataSSBOBindingPointIndex();
	m_MultipleSceneDepthShader[vTreeTypeIndex]->use();
	m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("planeOrTree", vPlaneOrTree);
	m_MultipleSceneDepthShader[vTreeTypeIndex]->setFloat("time", vTime);
	m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("frameIndex", vFrameIndex);
	m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("sumFaceVerticesBeforeEndMesh", getSumFaceVerticesBeforeEndMesh(vTreeTypeIndex));
	m_MultipleSceneDepthShader[vTreeTypeIndex]->setInt("waveMap", vWaveMap);
	//Draw(*m_MultipleSceneDepthShader[vTreeTypeIndex], *m_MultipleTreeModel[vTreeTypeIndex]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CInitMultipleTypeTree::RenderingModel(int vTreeTypeIndex, unsigned int vdepthMap, int vPlaneOrTree, float vTime, int vFrameIndex, int vWaveMap, int vBendScale, int vPrimaryOffsetScale,bool vshadows)
{
	m_MultipleTreeModel[vTreeTypeIndex]->UpdataSSBOBindingPointIndex();
	m_MultipleSceneShadowShader[vTreeTypeIndex]->use();
	glActiveTexture(GL_TEXTURE8);
    auto temp = glGetUniformLocation(m_MultipleSceneShadowShader[vTreeTypeIndex]->getID(), "depthMap");
    glUniform1i(temp, 8);
	glBindTexture(GL_TEXTURE_2D, vdepthMap);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("planeOrTree", vPlaneOrTree);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setFloat("time", vTime);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("frameIndex", vFrameIndex);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("sumFaceVerticesBeforeEndMesh", getSumFaceVerticesBeforeEndMesh(vTreeTypeIndex));
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("waveMap", vWaveMap);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("bendScale", vBendScale);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("primaryOffsetScale", vPrimaryOffsetScale);
	m_MultipleSceneShadowShader[vTreeTypeIndex]->setInt("shadows", vshadows);
	//Draw(*m_MultipleSceneShadowShader[vTreeTypeIndex], *m_MultipleTreeModel[vTreeTypeIndex]);
}

void CInitMultipleTypeTree::ComputeNormal(int vTreeTypeIndex)
{
	m_MultipleTreeModel[vTreeTypeIndex]->UpdataSSBOCompShaderNormalBindingPointIndex();
	m_MultipleTreeModel[vTreeTypeIndex]->ComputerShaderCalculateNormal(*(m_MultipleCompCalculateNormalShader[vTreeTypeIndex]));
}

void CInitMultipleTypeTree::Draw(CShader & vShader, CSence& vModel)
{
	vShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.getZoom()), (float)m_SCRWidth / (float)m_SCRHeight, 0.1f, 100.0f);
	glm::mat4 view = m_Camera.getViewMatrix();
	vShader.setMat4("projection", projection);
	vShader.setMat4("view", view);
	vShader.setVec3("camPos", m_Camera.getPosition());
	vModel.draw(vShader,Common::DrawType::TRIANGLES);
}

void CInitMultipleTypeTree::setTerrainPara(std::vector<double> & vTerrainHeigth, double vTerrainX, double vTerrainZ, double XDensityScale, double ZDensityScale, double vTerrainHeightZDensity)
{
	m_TerrainHeightY = vTerrainHeigth;
	m_TerrainX = vTerrainX;
	m_TerrainZ = vTerrainZ;
	m_TerrainXDensityScale = XDensityScale;
	m_TerrainZDensityScale = ZDensityScale;
	m_TerrainHeightZDensity = vTerrainHeightZDensity;
};
