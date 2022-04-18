#include "Sence.h"

CSence::CSence(const CMesh& vMesh)
{
	m_Meshes.push_back(vMesh);
}

//CSence::~CSence()
//{
//    //delete m_Mesh;
//    //delete m_DeformationU;
//    //delete m_Normal;
//    //delete m_TreeFileAndFrameIndex;
//    //delete m_InstanceDumMat;
//   // delete m_InstanceNoramMatrix;
//}

//****************************************************************************************************
//FUNCTION:
void CSence::draw(const CShader& vShader,Common::DrawType vDrawType)
{
    switch (vDrawType)
    {
    case Common::DrawType::TRIANGLES:
        for (auto& Mesh : m_Meshes)
        {
            Mesh.draw(vShader);
        }
        break;
    case Common::DrawType::LINES:
        for (auto& Mesh : m_Meshes)
        {
            Mesh.drawLine(vShader);
        }
        break;
    default:
        break;
    }
}

//fixme递进加载依旧依靠树的旋转平移
void CSence::setMeshRotation(std::vector<float> &vRotations, std::vector<std::pair<double, double>>& vTransFormations, float vScaleNumber, int vTreesNumber)
{
	m_InstanceTreeNumber = vTreesNumber;
	m_TransFormations = vTransFormations;
	glm::mat4*temp = translateTreePosition();
	
	m_SetRotation = vRotations;
	//specificTreeRotation(m_SetRotation, temp);
	//specificTreeZTransform(vYTransFormations, temp);
	// randomRotation(temp);
	//setScaleMesh(vScaleNumber, temp);

	m_InstanceDumMat = temp;

	for (auto& Mesh : m_Meshes)
	{
		Mesh.setRotation(temp, m_InstanceTreeNumber);
	}
}

//别问为什么写成这样，我也想知道为啥写成这样了....
//越看越恶心，这玩意居然是我写出来的
void CSence::UpdataYAxis(std::vector<double>& vYTransFormations)
{
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, vYTransFormations[i], 0.0f));
		m_InstanceDumMat[i] = model *m_InstanceDumMat[i];
	}

	for (auto& Mesh : m_Meshes)
	{
		Mesh.setRotation(m_InstanceDumMat, m_InstanceTreeNumber);
	}

}

void CSence::setObjectPositionAndRotation(glm::vec3 vPosition, float vRotation)
{
    glm::mat4 model =glm::mat4(1.0f);
    model=glm::translate(model, vPosition);
    m_InstanceMat = model;
    for (auto& Mesh : m_Meshes)
    {
        Mesh.setRotation(&model, 1);
    }
}

void CSence::setObjectPositionAndRotation(glm::mat4 & vMat)
{
    m_InstanceMat = vMat;
    for (auto& Mesh : m_Meshes)
    {
        Mesh.setRotation(&vMat, 1);
    }
}

//真tm的离谱....我写的是什么东西服了,把instancedraw和正常draw写一起了
void CSence::setTerrainMesh(std::vector<double>& vYTransFormations)
{
	m_InstanceTreeNumber = 1;
	glm::mat4* temp = new glm::mat4(1.0f);
	specificTreeZTransform(vYTransFormations, temp);
	m_InstanceMat = temp[0];
	for (auto& Mesh : m_Meshes)
	{
		Mesh.setRotation(temp, m_InstanceTreeNumber);
	}

}

void CSence::setScaleMesh(float vScale, glm::mat4 * vmodelMatrices)
{
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		vmodelMatrices[i] = glm::scale(vmodelMatrices[i], glm::vec3(vScale, vScale, vScale));
	}
}

//void CSence::setMeshRotationRelationWindFieldAndTreeDirection(std::vector<float> &vRotations)
//{
//    glm::mat4*temp = specificTreeRotation(vRotations);
//
//    for (auto& Mesh : m_Meshes)
//    {
//        Mesh.setSpecificRotation(temp);
//    }
//}
//

void CSence::specificTreeZTransform(std::vector<double> &vYTransFormations, glm::mat4* vmodelMatrces)
{
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		vmodelMatrces[i] = glm::translate(vmodelMatrces[i], glm::vec3(0.0f,vYTransFormations[i],0.0f));
	}
}

void CSence::specificTreeRotation(std::vector<float> &vRotations, glm::mat4* vmodelMatrces)
{
	glm::mat4* modelMatrices = new glm::mat4[m_InstanceTreeNumber];
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		//tempRandom = -120;
		//tempRandom = 0;
		//m_Angles.push_back(Common::SForceDirection(0, tempRandom));

		m_Angles.push_back(Common::SForceDirection(0, vRotations[i]));
		vmodelMatrces[i] = glm::rotate(vmodelMatrces[i], glm::radians(vRotations[i]), glm::vec3(0.0, 1.0, 0.0));
	}
}

glm::mat4* CSence::translateTreePosition()
{
	glm::mat4* modelMatrices = new glm::mat4[m_InstanceTreeNumber];

	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(translate[i], -0.5f,0));
		//0.45
		//0.2 -0.5 0.2w
		glm::vec3 tempTranslatePosition((m_TransFormations[i].first - Common::AllTreesNumber / 2) * 0.15, 0.0f, (m_TransFormations[i].second - Common::AllTreesNumber / 2) * 0.15);

        model = glm::translate(model, tempTranslatePosition);

        m_TreePositions.push_back(tempTranslatePosition);

		//model = glm::translate(model, glm::vec3((m_TransFormations[i].first - Common::AllTreesNumber / 2)*0.075, -0.5f, (m_TransFormations[i].second - Common::AllTreesNumber / 2)*0.075));
		modelMatrices[i] = model;
	}
	return modelMatrices;
}

void CSence::randomRotation(glm::mat4* vmodelMatrces)
{
	//生成一个Common::TreesNumber*Common::TreesNumber大小的网格，在网格内产生树的位置，并在一个网格上继续增加随机位置
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		srand(time(0));
		//因与搜索的风场与树角度有关，最好给定数据集，否则有可能产生搜索不到数据的情况
		float tempRandom = (float)RandomGenerate();
		vmodelMatrces[i] = glm::rotate(vmodelMatrces[i], glm::radians(tempRandom), glm::vec3(0.0, 1.0, 0.0));
		//glm::vec3 tempScale = GenerateRamdomScale();
		//vmodelMatrces[i] = glm::scale(model, vmodelMatrces[i]);
		//model = glm::scale(model, tempScale);
	}
}

void CSence::randomScaleMesh(glm::mat4 * vmodelMatrices)
{
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		glm::vec3 tempScale = GenerateRamdomScale();
		vmodelMatrices[i] = glm::scale(vmodelMatrices[i], tempScale);
	}
}

void CSence::setMeshGroupAndAssimpIndex()
{
	int count = 0;
	m_AssimpVerticesNumber = 0;
	for (auto& Mesh : m_Meshes)
	{
		int lastCapacity = m_AssimpVerticesNumber;
		int nextCapacity = getGroupsIndex()[count].size();
		Mesh.setGroupAndAssimpIndex(m_GroupsIndex[count], lastCapacity, nextCapacity);
		m_AssimpVerticesNumber += nextCapacity;
		count++;
	}
}

//
void CSence::setGroupsIndex(CVegaFemFactory& vfemFactoryObject)
{
    //Sence加载的模型定点顺序与CVegaFemFactory中用vega加载的模型顺序不同

    std::vector<std::vector<int>> tempGroupsIndex= vfemFactoryObject.getModelTransformStruct()->getGroupsIndex();
    for (auto tempMesh : m_Meshes)
    {
        for (int i = 0; i < tempGroupsIndex.size(); i++)
        {
            if (tempMesh.getVertices().size() == tempGroupsIndex[i].size())
            {
                m_GroupsIndex.push_back(tempGroupsIndex[i]);
            }
        }
            
    }
	//m_GroupsIndex = vfemFactoryObject.getModelTransformStruct()->getGroupsIndex();
	if (Common::UseGeomOrCompCalculateNormal == false)
	{
		__setVertexRelatedFace();
	}
}

void CSence::setVerticesNumber(CVegaFemFactory & vfemFactoryObject)
{
	m_VerticesNumber = vfemFactoryObject.getModelTransformStruct()->getVerticesNumber();
}

void CSence::__setVertexRelatedFace()
{
    std::string filePath = m_FileDirectory + "/ModelVertexAndFaceRelation.txt";
    //顶点面片关系文件存在则直接使用不存在则创建
    //bool ifFIlexist = _access(filePath.c_str(), 0);
    if (_access(filePath.c_str(), 0) == 0)
    {
        std::string lineString;
        std::ifstream infile(filePath,std::ios::binary);
        std::getline(infile, lineString);
        int FaceNumber = atoi(lineString.c_str());
        std::getline(infile, lineString);
        std::istringstream FaceNumberStream(lineString);
        for (int i = 0; i < FaceNumber; i++)
        {
            int temp;
            FaceNumberStream >> temp;
            m_EachVertexWithFaceNumber.push_back(temp);
        }

        std::getline(infile, lineString);
        int FaceFirstIndex = atoi(lineString.c_str());
        std::getline(infile, lineString);
        std::istringstream FaceFirstIndexStream(lineString);
        for (int i = 0; i < FaceFirstIndex; i++)
        {
            int temp;
            FaceFirstIndexStream >> temp;
            m_EachVertexWithFaceFirstIndex.push_back(temp);
        }

        std::getline(infile, lineString);
        int AllVertexRelated = atoi(lineString.c_str());
        std::getline(infile, lineString);
        std::istringstream AllVertexRelatedStream(lineString);
        for (int i = 0; i < AllVertexRelated; i++)
        {
            int temp;
            AllVertexRelatedStream >> temp;
            m_AllVertexRelatedFaceIndex.push_back(temp);
        }
        infile.close();
    }
    else if(_access(filePath.c_str(), 0)==-1) {//文件不存在创建
         std::cout <<"\n"<< "VertexRelatedFace not exist" << filePath << std::endl;
         std::cout << "start create ModelVertexAndFaceRelationFile" << std::endl;
        int SumRelatedPosition = 0;
        int m_GroupIndexSum = 0;
        for (int i = 0; i < m_GroupsIndex.size(); i++)
        {
            int GroupAllFaceNumber = m_GroupsIndex[i].size() / 3;

            //复制后的顶点id与所有顶点的faceid比较，如果相同取出其面的索引以及相关的面的总数
            for (int VertexIndex = 0; VertexIndex < m_GroupsIndex[i].size(); VertexIndex++)
            {
                m_EachVertexWithFaceFirstIndex.push_back(SumRelatedPosition);
                int RelatedFaceNumber = 0;
                for (int AllVertexIndex = 0; AllVertexIndex < m_GroupsIndex[i].size(); AllVertexIndex++)
                {
                    if (m_GroupsIndex[i][VertexIndex] == m_GroupsIndex[i][AllVertexIndex])
                    {
                        //在第几个面中
                        m_AllVertexRelatedFaceIndex.push_back(m_GroupIndexSum + AllVertexIndex / 3);
                        //m_AllVertexRelatedFaceIndex.push_back( AllVertexIndex / 3);
                        RelatedFaceNumber++;
                    }
                }
                m_EachVertexWithFaceNumber.push_back(RelatedFaceNumber);
                SumRelatedPosition += RelatedFaceNumber;

            }
            m_GroupIndexSum += GroupAllFaceNumber;
        }

        std::fstream outFile;
        outFile.open(filePath, std::ios::out | std::ios::binary);
        outFile << m_EachVertexWithFaceNumber.size() << std::endl;
        for (int i = 0; i<m_EachVertexWithFaceNumber.size(); i++)
        {
            outFile << m_EachVertexWithFaceNumber[i] << " ";
        }
        outFile << "\n";

        outFile << m_EachVertexWithFaceFirstIndex.size() << std::endl;
        for (int i = 0; i < m_EachVertexWithFaceFirstIndex.size(); i++)
        {
            outFile << m_EachVertexWithFaceFirstIndex[i] << " ";
        }
        outFile << "\n";

        outFile << m_AllVertexRelatedFaceIndex.size() << std::endl;
        for (int i = 0; i < m_AllVertexRelatedFaceIndex.size(); i++)
        {
            outFile << m_AllVertexRelatedFaceIndex[i] << " ";
        }
        outFile << "\n";
        outFile.close();
        std::cout << "create File End" << std::endl;
    }
   

}

void CSence::__changeObjMeshStruct2Charptr(int vOutputMaterials)
{
	std::string materialFilename;
	std::string materialFilenameLocal;
	//bool materialFileExist=false;

	if (vOutputMaterials && (m_Mesh->getNumMaterials() == 0))
		vOutputMaterials = 0;

	if (vOutputMaterials)
	{
		materialFilename = m_Mesh->getFilename() + ".mtl";
		// remove directory part from materialFilename
		char * materialFilenameTempC = (char*)materialFilename.c_str();
		char * beginString = materialFilenameTempC;
		// seek for last '/'
		for (unsigned int i = 0; i < strlen(materialFilenameTempC); i++)
			if ((materialFilenameTempC[i] == '/') || (materialFilenameTempC[i] == '\\'))
				beginString = &materialFilenameTempC[i + 1];

		materialFilenameLocal = std::string(beginString);
	}

	int numTriangles = 0;
	for (unsigned int i = 0; i < m_Mesh->getNumGroups(); i++)
		numTriangles += m_Mesh->getGroup(i).getNumFaces();
	std::stringstream ss;

	ss << "# Generated by the ObjMesh class" << std::endl;
	ss << "# Number of vertices: " << m_Mesh->getNumVertices() << std::endl;
	ss << "# Number of texture coordinates: " << m_Mesh->getNumTextureCoordinates() << std::endl;
	ss << "# Number of normals: " << m_Mesh->getNumNormals() << std::endl;
	ss << "# Number of faces: " << numTriangles << std::endl;
	ss << "# Number of groups: " << m_Mesh->getNumGroups() << std::endl;

	if (vOutputMaterials)
		ss << std::endl << "mtllib " << materialFilenameLocal << std::endl << std::endl;

	int maxPrecision = std::numeric_limits<long double>::digits10 + 1;
	int pcs = (-1 > maxPrecision ? maxPrecision : -1);
	if (pcs > 0)
		ss << std::setprecision(pcs);
	for (unsigned int i = 0; i < m_Mesh->getNumVertices(); i++)
	{
		Vec3d pos = m_Mesh->getPosition(i);
		double x = m_RestVertexs[3 * i + 0];
		double y = m_RestVertexs[3 * i + 1];
		double z = m_RestVertexs[3 * i + 2];
		ss << "v " << (pos[0] - x) << " " << (pos[1] - y) << " " << (pos[2] - z) << std::endl;
	}
	for (unsigned int i = 0; i < m_Mesh->getNumTextureCoordinates(); i++)
	{
		Vec3d texCoord_ = m_Mesh->getTextureCoordinate(i);
		ss << "vt " << texCoord_[0] << " " << texCoord_[1] << std::endl;
	}
	// normals...
	for (unsigned int i = 0; i < m_Mesh->getNumNormals(); i++)
	{
		Vec3d normal_ = m_Mesh->getNormal(i);
		ss << "vn " << normal_[0] << " " << normal_[1] << " " << normal_[2] << std::endl;
	}
	// groups and faces...
	for (unsigned int i = 0; i < m_Mesh->getNumGroups(); i++)
	{
		ss << "g " << m_Mesh->getGroup(i).getName() << std::endl;
		if (vOutputMaterials)
		{
			ss << "usemtl " << m_Mesh->getMaterial(m_Mesh->getGroup(i).getMaterialIndex()).getName() << std::endl;
		}
		for (unsigned int iFace = 0; iFace < m_Mesh->getGroup(i).getNumFaces(); iFace++)
		{
			ObjMesh::Face face = m_Mesh->getGroup(i).getFace(iFace); // get face whose number is iFace

			ss << "f";
			if (face.getNumVertices() < 3)
				std::cout << "Warning: encountered a face (group=" << i << ",face=" << iFace << ") with fewer than 3 vertices." << std::endl;

			for (unsigned int iVertex = 0; iVertex < face.getNumVertices(); iVertex++)
			{
				ObjMesh::Vertex vertex = face.getVertex(iVertex);
				ss << " " << int(vertex.getPositionIndex() + 1);
				if (vertex.hasTextureCoordinateIndex() || vertex.hasNormalIndex())
				{
					ss << "/";
					if (vertex.hasTextureCoordinateIndex())
						ss << int(vertex.getTextureCoordinateIndex() + 1);
					if (vertex.hasNormalIndex())
					{
						ss << "/";
						if (vertex.hasNormalIndex())
							ss << int(vertex.getNormalIndex() + 1);
					}
				}
			}
			ss << std::endl;
		}
	}
	m_Data = ss.str();
	m_DataSize = strlen((char*)ss.str().c_str());
}

//****************************************************************************************************
//FUNCTION:
void CSence::__loadModel(const std::string& vModelPath, bool vloadNormalizeModelAndFaceNormal)
{
	Assimp::Importer Importer;
	const aiScene* pScene;
	if (vloadNormalizeModelAndFaceNormal == true)
	{
		aiPropertyStore* Porps = aiCreatePropertyStore();
		aiSetImportPropertyInteger(Porps, "PP_PTV_NORMALIZE", 1);
		pScene = (aiScene*)aiImportFileExWithProperties(vModelPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices, NULL, Porps);
		aiReleasePropertyStore(Porps);
	}
	else
	{
		pScene = Importer.ReadFile(vModelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP:: " << Importer.GetErrorString() << std::endl;
			return;
		}
	}
	setFileDirectiory(vModelPath.substr(0, vModelPath.find_last_of('/')));
	__processNode(pScene->mRootNode, pScene, false);
}

void CSence::__loadModelFromMemory()
{
	const aiScene* pScene = NULL;
	Assimp::Importer *imp = new Assimp::Importer();
	int Flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	pScene = imp->ReadFileFromMemory((unsigned char*)m_Data.c_str(), m_DataSize, Flags);
	__processNode(pScene->mRootNode, pScene, true);
}

//****************************************************************************************************
//FUNCTION:
void CSence::__processNode(const aiNode* vNode, const aiScene* vScene, bool vSaveDeformationOrLoadData)
{
	for (unsigned int i = 0; i < vNode->mNumMeshes; i++)
	{
		aiMesh* Mesh = vScene->mMeshes[vNode->mMeshes[i]];
		/*auto temp = __processMesh(Mesh, vScene);
		m_Meshes.push_back(temp);*/
		if (vSaveDeformationOrLoadData == false)
			m_Meshes.push_back(__processMesh(Mesh, vScene));
		else
		{
			__processSaveDeformation(Mesh, vScene);
		} 
	}
	//group的个数
	for (unsigned int i = 0; i < vNode->mNumChildren; i++)
	{
		__processNode(vNode->mChildren[i], vScene, vSaveDeformationOrLoadData);
	}
}

//****************************************************************************************************
//FUNCTION:针对每一个group进行处理
CMesh CSence::__processMesh(const aiMesh* vMesh, const aiScene* vScene)
{
	std::vector<Common::SVertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<Common::STexture> Textures;

	for (unsigned int i = 0; i < vMesh->mNumVertices; i++)
	{
		Common::SVertex Vertex;
		Vertex.Position = Common::vec3_cast(vMesh->mVertices[i]);
		Vertex.Normal = Common::vec3_cast(vMesh->mNormals[i]);
		if (vMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = vMesh->mTextureCoords[0][i].x;
			vec.y = vMesh->mTextureCoords[0][i].y;
			Vertex.TexCoords = vec;
		}
		else
			Vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		if (vMesh->HasTangentsAndBitangents())
		{
			Vertex.Tangent = Common::vec3_cast(vMesh->mTangents[i]);
			Vertex.Bitangent = Common::vec3_cast(vMesh->mBitangents[i]);
			Common::ModelHaveTangentAndBitangent = true;
		}
		else
			Common::ModelHaveTangentAndBitangent = false;
		Vertices.push_back(Vertex);
	}

	for (unsigned int i = 0; i < vMesh->mNumFaces; i++)
	{
		const aiFace Face = vMesh->mFaces[i];

		for (unsigned int k = 0; k < Face.mNumIndices; k++)
			Indices.push_back(Face.mIndices[k]);
	}
	aiMaterial* material = vScene->mMaterials[vMesh->mMaterialIndex];
	// 1. diffuse maps
	std::vector<Common::STexture> DiffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	Textures.insert(Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());
	/* 2. specular maps*/
	std::vector<Common::STexture> SpecularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	Textures.insert(Textures.end(), SpecularMaps.begin(), SpecularMaps.end());
	// 3. normal maps
	std::vector<Common::STexture> NormalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	Textures.insert(Textures.end(), NormalMaps.begin(), NormalMaps.end());
	////// 4. height maps
	std::vector<Common::STexture> OpacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
	Textures.insert(Textures.end(), OpacityMaps.begin(), OpacityMaps.end());

	return CMesh(Vertices, Indices, Textures);
}

//****************************************************************************************************
//FUNCTION:
void CSence::__processSaveDeformation(const aiMesh* vMesh, const aiScene* vScene)
{
	std::vector<glm::vec3> Vertices;
	for (unsigned int i = 0; i < vMesh->mNumVertices; i++)
	{
		Vertices.push_back(Common::vec3_cast(vMesh->mVertices[i]));
	}
	m_EachFrameOfGroupData.push_back(Vertices);
}

//****************************************************************************************************
//FUNCTION:
unsigned int CSence::TextureFromFile(const char *vPath, const std::string &vDirectory, bool vGamma)
{
	std::string FileName = std::string(vPath);
	FileName = vDirectory + '/' + FileName;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(FileName.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << vPath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


//****************************************************************************************************
//FUNCTION:
std::vector<Common::STexture> CSence::loadMaterialTextures(aiMaterial *vMat, aiTextureType vType, std::string vTypeName)
{
	std::vector<Common::STexture> Textures;
	for (unsigned int i = 0; i < vMat->GetTextureCount(vType); i++)
	{
		aiString str;
		vMat->GetTexture(vType, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < m_Textures.size(); j++)
		{
			if (std::strcmp(m_Textures[j].path.data(), str.C_Str()) == 0)
			{
				Textures.push_back(m_Textures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Common::STexture Texture;
			Texture.id = TextureFromFile(str.C_Str(), this->m_FileDirectory);
			Texture.type = vTypeName;
			Texture.path = str.C_Str();
			Textures.push_back(Texture);
			m_Textures.push_back(Texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return Textures;
}

void CSence::getModelVertex()
{
	m_staticMeshVertex=new glm::vec3[m_AssimpVerticesNumber];
	int tempIndex = 0;
	/*for (int i = 0; i < m_Meshes[0].getVertices().size(); i++)
	{
		m_staticMeshVertex[i] = m_Meshes[0].getVertices()[i].Position;
	}*/
	for (auto& Mesh : m_Meshes)
	{
		for (int i = 0; i < Mesh.getVertices().size(); i++)
		{
			m_staticMeshVertex[tempIndex]=Mesh.getVertices()[i].Position;
			tempIndex++;
		}
	}
}

void CSence::initSSBODeformationDeltaU(CVegaFemFactory & vFem, int vFileNumber)
{
	//帧数
	m_FrameNums = vFem.getFileFrames(0)->Frames.size();
	//obj模型的顶点数
	m_VertexNums = vFem.getFileFrames(0)->Frames[0].BaseFileDeformations.size();

	m_FileNumber = vFileNumber;

	std::cout << m_FrameNums << " " << m_VertexNums << std::endl;
	m_DeltaDeformationU = new glm::vec4[m_FrameNums*m_VertexNums*vFileNumber];
    //m_DeltaDeformationU = new glm::vec4[0];
	int count = 0;
	for (int fileIndex = 0; fileIndex < vFileNumber; fileIndex++)
		for (int frameIndex = 0; frameIndex < vFem.getFileFrames(fileIndex)->Frames.size(); frameIndex++)
		{
			Common::SFileData* tempFrame = &(vFem.getFileFrames(fileIndex)->Frames[frameIndex]);
			for (int k = 0; k < tempFrame->BaseFileDeformations.size(); k++)
			{
				m_DeltaDeformationU[fileIndex*m_FrameNums*m_VertexNums + frameIndex * m_VertexNums + k] = glm::vec4(tempFrame->BaseFileDeformations[k],0.0f);
				count++;
			}
		}
	std::cout << "counter" << count << std::endl;
}

void CSence::initSSBODeformationU()
{
	setMeshGroupAndAssimpIndex();
	getModelVertex();
	m_DeformationU = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
	for (int treeNumber = 0; treeNumber<m_InstanceTreeNumber; treeNumber++)
	{
		for (int index = 0; index < m_AssimpVerticesNumber; index++)
		{
			m_DeformationU[treeNumber*m_AssimpVerticesNumber + index] = glm::vec4(m_staticMeshVertex[index],0.0f);
		}
	}
	//memset(m_DeformationU, 0, sizeof(glm::vec4)*m_InstanceTreeNumber*m_AssimpVerticesNumber);
}

void CSence::initSSBOTreeFileAndFrameIndex(const int vTreeNumber)
{
	m_TreeFileAndFrameIndex = new glm::ivec2[vTreeNumber];
}

void CSence::setTreeNumber(const int vInstanceTreeNumber)
{
	m_InstanceTreeNumber = vInstanceTreeNumber;
}

void CSence::setSSBO4GenBufferUDeformationAndIndex(CShader& vShader, const int vTreeTypeIndex)
{
	std::cout << "setSSBO4UDeformationAndIndex" << std::endl;
	std::cout << "ShaderId" << vShader.getID() << std::endl;
	std::cout << m_FrameNums * m_VertexNums*m_FileNumber << std::endl;

	//for (auto i = 0; i < 3; i++)
	//	m_SSBO_Binding_Point_Index.push_back(3 * vTreeTypeIndex + i);

	vShader.use();
	//设置所有DeltaU数据
	GLint SSBOBinding = 0, BlockDataSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize);

	//初始化SSBO
	glGenBuffers(1, &m_DeltaUSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DeltaUSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_FrameNums*m_VertexNums*m_FileNumber), m_DeltaDeformationU, GL_STATIC_DRAW);

	//SSBOBuffer connect bindingpoint
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[0], m_DeltaUSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_DeltaUSSBO);

	//设置DelataU用来存储生成树的当前型变量
	GLint SSBOBindingfirst = 0, BlockDataSizefirst = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBindingfirst);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSizefirst);

	glGenBuffers(1, &m_UdeformationSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_DeformationU, GL_DYNAMIC_DRAW);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[1], m_UdeformationSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_UdeformationSSBO);

	//设置TreeFile和FrameIndex
	GLint SSBOBinding2 = 0, BlockDataSize2 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding2);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize2);

	glGenBuffers(1, &m_TreeFileAndFrameSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TreeFileAndFrameSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::ivec2)*(m_InstanceTreeNumber), m_TreeFileAndFrameIndex, GL_DYNAMIC_DRAW);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[2], m_TreeFileAndFrameSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TreeFileAndFrameSSBO);

	//设置法线
	if (Common::UseGeomOrCompCalculateNormal == false)
	{
		m_Normal = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
		memset(m_Normal, 0, sizeof(glm::vec4)*m_InstanceTreeNumber*m_AssimpVerticesNumber);

		GLint SSBOBinding3 = 0, BlockDataSize3 = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding3);
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize3);

		glGenBuffers(1, &m_NormalSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_NormalSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_Normal, GL_DYNAMIC_DRAW);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_NormalSSBO);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CSence::UpdataSSBOBindingPointIndex()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_DeltaUSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_UdeformationSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TreeFileAndFrameSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_NormalSSBO);
}

void CSence::setSSBO4UDeformationAndIndex(CShader& vShader)
{
	//设置所有DeltaU数据
	vShader.use();
	GLuint shader_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeltaDeformationArray");
	GLint SSBOBinding = 0, BlockDataSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize);

	std::cout << "setSSBO4UDeformationAndIndex" << std::endl;
	std::cout << "ShaderId" << vShader.getID() << std::endl;
	std::cout << m_FrameNums * m_VertexNums*m_FileNumber << std::endl;
	//初始化SSBO
	glGenBuffers(1, &m_DeltaUSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DeltaUSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_FrameNums*m_VertexNums*m_FileNumber), m_DeltaDeformationU, GL_STATIC_DRAW);

	//shader和点连接
	GLuint ssbo_binding_point_index = 1;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, m_DeltaUSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_index, ssbo_binding_point_index);

	//设置DelataU用来存储生成树的当前型变量
	//shader和点连接
	GLuint shader_delta_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeformationArray");
	GLint SSBOBinding1 = 0, BlockDataSize1 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding1);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize1);

	glGenBuffers(1, &m_UdeformationSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_DeformationU, GL_DYNAMIC_DRAW);
	GLuint deltassbo_binding_point_index = 2;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, deltassbo_binding_point_index, m_UdeformationSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_delta_index, deltassbo_binding_point_index);

	//设置TreeFile和FrameIndex
	GLuint shader_file_frame_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "IndexArray");
	GLint SSBOBinding2 = 0, BlockDataSize2 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding2);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize2);

	glGenBuffers(1, &m_TreeFileAndFrameSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TreeFileAndFrameSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::ivec2)*(m_InstanceTreeNumber), m_TreeFileAndFrameIndex, GL_DYNAMIC_DRAW);
	GLuint file_frame_ssbo_binding_point_index = 3;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, file_frame_ssbo_binding_point_index, m_TreeFileAndFrameSSBO);
	//点和shader的连接
	//glShaderStorageBlockBinding(vShader.getID(), shader_file_frame_index, file_frame_ssbo_binding_point_index);



}

void CSence::setSSBOUdeformationAndIndx4ShadowMapShader(const CShader& vShader)
{
	//设置所有DeltaU数据
	GLuint shader_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeltaDeformationArray");

	//shader和点连接
	GLuint ssbo_binding_point_index = 1;
	//点和SSBO的连接
	//!!!!!!!使用了同一个BufferData
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, m_DeltaUSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_index, ssbo_binding_point_index);

	//设置DelataU用来存储生成树的当前型变量
	//shader和点连接
	GLuint shader_delta_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeformationArray");

	GLuint deltassbo_binding_point_index = 2;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, deltassbo_binding_point_index, m_UdeformationSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_delta_index, deltassbo_binding_point_index);

	//设置TreeFile和FrameIndex
	GLuint shader_file_frame_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "IndexArray");
	GLuint file_frame_ssbo_binding_point_index = 3;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, file_frame_ssbo_binding_point_index, m_TreeFileAndFrameSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_file_frame_index, file_frame_ssbo_binding_point_index);
}

void CSence::UpdataSSBOMeshTreeAndFrameIndex(std::vector<std::pair<int, int>>& vTreeFileAndFrameIndex)
{
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		m_TreeFileAndFrameIndex[i] = glm::ivec2(vTreeFileAndFrameIndex[i].first, vTreeFileAndFrameIndex[i].second);
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TreeFileAndFrameSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TreeFileAndFrameSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::ivec2)*(m_InstanceTreeNumber), m_TreeFileAndFrameIndex);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void CSence::initComputerSSBONormalRelatedData(ComputerShader& vShader, const int vTreeTypeIndex)
{
	
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_UdeformationSSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	//set NormalRealted
	//-----------------
	vShader.use();
	GLuint shader_VerwithFaceNumber_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "EachVertexWithFaceNumber");
	GLint SSBOBinding1 = 0, BlockDataSize1 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding1);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize1);

	glGenBuffers(1, &m_VertexWithFaceNumberSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VertexWithFaceNumberSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*(m_EachVertexWithFaceNumber.size()), &m_EachVertexWithFaceNumber[0], GL_DYNAMIC_DRAW);
	GLuint VerwithFaceNumberssbo_binding_point_index = 5;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VerwithFaceNumberssbo_binding_point_index, m_VertexWithFaceNumberSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_VerwithFaceNumber_index, VerwithFaceNumberssbo_binding_point_index);
	//--------------------------
	GLuint shader_VertexWithFaceFirstIndex_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "EachVertexWithFaceFirstIndex");
	GLint SSBOBinding2 = 0, BlockDataSize2 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding2);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize2);

	glGenBuffers(1, &m_VertexWithFaceFirstSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VertexWithFaceFirstSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*(m_EachVertexWithFaceFirstIndex.size()), &m_EachVertexWithFaceFirstIndex[0], GL_DYNAMIC_DRAW);
	GLuint VertexWithFaceFirstIndexssbo_binding_point_index = 6;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, VertexWithFaceFirstIndexssbo_binding_point_index, m_VertexWithFaceFirstSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_VertexWithFaceFirstIndex_index, VertexWithFaceFirstIndexssbo_binding_point_index);
	//--------------------------------
	GLuint shader_AllVertexRelatedFaceIndex_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "AllVertexWithFaceIndex");
	GLint SSBOBinding3 = 0, BlockDataSize3 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding3);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize3);

	glGenBuffers(1, &m_AllVertexRelatedFaceSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_AllVertexRelatedFaceSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)*(m_AllVertexRelatedFaceIndex.size()), &m_AllVertexRelatedFaceIndex[0], GL_DYNAMIC_DRAW);
	GLuint AllVertexRelatedFaceIndexssbo_binding_point_index = 7;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, AllVertexRelatedFaceIndexssbo_binding_point_index, m_AllVertexRelatedFaceSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_AllVertexRelatedFaceIndex_index, AllVertexRelatedFaceIndexssbo_binding_point_index);

	//------------------------------
	//set DeformationModel
	GLuint shader_delta_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "DeformationArray");

	GLuint deltassbo_binding_point_index = 2;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, deltassbo_binding_point_index, m_UdeformationSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_delta_index, deltassbo_binding_point_index);
	//----------
	//set Normal
	GLuint shader_Normal_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "bufferNormal");

	GLuint Normalssbo_binding_point_index = 4;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Normalssbo_binding_point_index, m_NormalSSBO);
	//点和shader的连接
	glShaderStorageBlockBinding(vShader.getID(), shader_Normal_index, Normalssbo_binding_point_index);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

//没有使用，当时想法是替换掉vert着色中的model matrix 的instance部分
void CSence::setSSBO4GenModelNormalMatrixData(glm::mat4 vmodelMatrix)
{
	m_InstanceNoramMatrix = new glm::mat4[m_InstanceTreeNumber];
	for (int i = 0; i < m_InstanceTreeNumber; i++)
	{
		//m_InstanceNoramMatrix[i]= glm::transpose(glm::inverse(vmodelMatrix*m_InstanceDumMat[i]));
		m_InstanceNoramMatrix[i] = vmodelMatrix*m_InstanceDumMat[i];
	}
}

//当时想法是替换掉vert着色中的model matrix 的instance部分,现在用在对Computer Shader中使用，
//改进可以继承同时对两个Shader的Model矩阵部分替换
void CSence::initSSBO4GenModelNormalMatrixBuffer(ComputerShader& vShader, const int vTreeTypeIndex)
{
	std::cout << "ShaderId" << vShader.getID() << std::endl;
	vShader.use();
	//设置所有DeltaU数据
	GLint SSBOBinding = 0, BlockDataSize = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize);

	//初始化SSBO
	glGenBuffers(1, &m_ModelNormalMatrixSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelNormalMatrixSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4)*(m_InstanceTreeNumber), m_InstanceNoramMatrix, GL_DYNAMIC_DRAW);

	GLuint ModelNormalMatrixssbo_binding_point_index = 8;
	//点和SSBO的连接
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ModelNormalMatrixssbo_binding_point_index, m_ModelNormalMatrixSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//----------------------------------------
	//GLuint shader_ModelNormalMatrix_index = glGetProgramResourceIndex(vShader.getID(), GL_SHADER_STORAGE_BLOCK, "EachTreeInstanceModelMatrix");
	//GLint SSBOBinding1 = 0, BlockDataSize1 = 0;
	//glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding1);
	//glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize1);

	//std::cout << "setSSBO4 NormalMatrix Buffer" << std::endl;
	//std::cout << "ShaderID" << vShader.getID() << std::endl;

	//glGenBuffers(1, &m_ModelNormalMatrixSSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ModelNormalMatrixSSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4)*(m_InstanceTreeNumber), m_InstanceNoramMatrix, GL_DYNAMIC_DRAW);
	//GLuint ModelNormalMatrixssbo_binding_point_index = 8;
	////点和SSBO的连接
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ModelNormalMatrixssbo_binding_point_index, m_ModelNormalMatrixSSBO);
	////点和shader的连接
	//glShaderStorageBlockBinding(vShader.getID(), shader_ModelNormalMatrix_index, ModelNormalMatrixssbo_binding_point_index);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

//每次调用前需要重新绑定一次（感觉如果这样的话正常情况在init时不需要绑定）
void CSence::UpdataSSBOCompShaderNormalBindingPointIndex()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_ModelNormalMatrixSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_NormalSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_UdeformationSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_VertexWithFaceNumberSSBO); 
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_VertexWithFaceFirstSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_AllVertexRelatedFaceSSBO);
}

//此处的index可能在多棵树时存在错误
//void CSence::initSSBO4GenBufferNormal(CShader& vShader, const int vTreeTypeIndex)
//{
//	m_Normal = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
//	memset(m_Normal, 0, sizeof(glm::vec4)*m_InstanceTreeNumber*m_AssimpVerticesNumber);
//	m_SSBO_Binding_Point_Index_Normal.push_back(4 * vTreeTypeIndex);
//
//	vShader.use();
//
//	GLint SSBOBindingfirst = 0, BlockDataSizefirst = 0;
//	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBindingfirst);
//	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSizefirst);
//
//	glGenBuffers(1, &m_NormalSSBO);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_NormalSSBO);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_Normal, GL_DYNAMIC_DRAW);
//
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index_Normal[0], m_NormalSSBO);
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//}

void CSence::ComputerShaderCalculateNormal(ComputerShader& vShader)
{
	vShader.use();
	glDispatchCompute(m_AssimpVerticesNumber / 3, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CSence::resetSSBO4UDeformation()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	for (int treeNumber = 0; treeNumber < m_InstanceTreeNumber; treeNumber++)
	{
		for (int index = 0; index < m_AssimpVerticesNumber; index++)
		{
			m_DeformationU[treeNumber*m_AssimpVerticesNumber + index] = glm::vec4(m_staticMeshVertex[index], 0.0f);
		}
	}
	//m_DeformationU = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_DeformationU);
}
////////////
void CSence::getModelHeight()
{
	//m_TerrainHeight.resize(3000 * 3000);
	//for (int i = 0; i < m_Meshes.size(); i++)
	//{
	//	Common::SVertex tempVertice=m_Meshes[i].getVertices;
	//	tempVertice.Position.x

	//}
}

void CSence::ClearDeformationDeltaU()
{
    delete[] m_DeltaDeformationU;
}

void CSence::ClearAllPointer()
{
    DELETEPointer(m_Mesh);
    DELETEPointer(m_DeformationU);
    DELETEPointer(m_Normal);
    DELETEPointer(m_TreeFileAndFrameIndex);
    DELETEPointer(m_InstanceDumMat);
    DELETEPointer(m_InstanceNoramMatrix);
}
