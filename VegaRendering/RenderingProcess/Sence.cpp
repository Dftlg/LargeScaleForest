#include "Sence.h"

CSence::CSence(const CMesh& vMesh)
{
	m_Meshes.push_back(vMesh);
}

//****************************************************************************************************
//FUNCTION:
void CSence::draw(const CShader& vShader)
{
	for (auto& Mesh : m_Meshes)
	{
		Mesh.draw(vShader);
	}

}

//fixme递进加载依旧依靠树的旋转平移
void CSence::setMeshRotation(std::vector<float> &vRotations, std::vector<std::pair<double, double>>& vTransFormations, float vScaleNumber, int vTreesNumber)
{
	m_InstanceTreeNumber = vTreesNumber;
	m_TransFormations = vTransFormations;
	glm::mat4*temp = translateTreePosition();
	
	m_SetRotation = vRotations;
	specificTreeRotation(m_SetRotation, temp);
	//specificTreeZTransform(vYTransFormations, temp);
	randomRotation(temp);
	setScaleMesh(vScaleNumber, temp);

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

		//glm::vec3 tempTranslatePosition(i*100.0f, 0.0f, 0.0f);
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
	m_GroupsIndex = vfemFactoryObject.getModelTransformStruct()->getGroupsIndex();
}

void CSence::setVerticesNumber(CVegaFemFactory & vfemFactoryObject)
{
	m_VerticesNumber = vfemFactoryObject.getModelTransformStruct()->getVerticesNumber();
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

void CSence::initSSBODeformationDeltaU(CVegaFemFactory & vFem, int vFileNumber)
{
	//帧数
	m_FrameNums = vFem.getFileFrames(0)->Frames.size();
	//obj模型的顶点数
	m_VertexNums = vFem.getFileFrames(0)->Frames[0].BaseFileDeformations.size();

	m_FileNumber = vFileNumber;

	std::cout << m_FrameNums << " " << m_VertexNums << std::endl;
	m_DeltaDeformationU = new glm::vec4[m_FrameNums*m_VertexNums*vFileNumber];
	int count = 0;
	for (int fileIndex = 0; fileIndex < vFileNumber; fileIndex++)
		for (int frameIndex = 0; frameIndex < vFem.getFileFrames(fileIndex)->Frames.size(); frameIndex++)
		{
			Common::SFileData* tempFrame = &(vFem.getFileFrames(fileIndex)->Frames[frameIndex]);
			for (int k = 0; k < tempFrame->BaseFileDeformations.size(); k++)
			{
				m_DeltaDeformationU[fileIndex*m_FrameNums*m_VertexNums + frameIndex * m_VertexNums + k] = glm::vec4(tempFrame->BaseFileDeformations[k], 0.0f);
				count++;
			}
		}
	std::cout << "counter" << count << std::endl;
}

void CSence::initSSBODeformationU()
{
	setMeshGroupAndAssimpIndex();
	m_DeformationU = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
	memset(m_DeformationU, 0, sizeof(glm::vec4)*m_InstanceTreeNumber*m_AssimpVerticesNumber);
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

	for (auto i = 0; i < 3; i++)
		m_SSBO_Binding_Point_Index.push_back(3 * vTreeTypeIndex + i);

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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[0], m_DeltaUSSBO);

	//设置DelataU用来存储生成树的当前型变量
	GLint SSBOBindingfirst = 0, BlockDataSizefirst = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBindingfirst);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSizefirst);

	glGenBuffers(1, &m_UdeformationSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*(m_AssimpVerticesNumber*m_InstanceTreeNumber), m_DeformationU, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[1], m_UdeformationSSBO);

	//设置TreeFile和FrameIndex
	GLint SSBOBinding2 = 0, BlockDataSize2 = 0;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &SSBOBinding2);
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &BlockDataSize2);

	glGenBuffers(1, &m_TreeFileAndFrameSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TreeFileAndFrameSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::ivec2)*(m_InstanceTreeNumber), m_TreeFileAndFrameIndex, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_SSBO_Binding_Point_Index[2], m_TreeFileAndFrameSSBO);
}

void CSence::UpdataSSBOBindingPointIndex()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_DeltaUSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_UdeformationSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_TreeFileAndFrameSSBO);
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
	// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void CSence::resetSSBO4UDeformation()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_UdeformationSSBO);
	m_DeformationU = new glm::vec4[m_InstanceTreeNumber*m_AssimpVerticesNumber];
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
