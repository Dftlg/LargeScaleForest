#include"CubicVeg.h"


//     3 - - - 2
//    /|      /|
//   7 - - - 6 |       y
//   | |     | |       |
//   | 0 - - | 1       |_ _ _x
//   |/      |/       /
//   4 - - - 5       z
//DrawLine顺序，0-1，1-2，2-3，3-0
// 4-5,5-6,6-7,7-4
//0-4,1-5,2-6,3-7

//DrawTriangle顺序
//// front
//0, 1, 3,
//1, 2, 3,
// back
//4, 5, 7,
//5, 6, 7,
// right
//0, 1, 4,
//1, 4, 5,
// left
//2, 3, 7,
//2, 6, 7,
// top
//0, 3, 4,
//3, 4, 7,
// bottom
//1, 2, 5,
//2, 5, 6

//对于一个体素周围相关体素有27个，一个是自身，8个是顶点相关省略，6个对面相关，12个对边相关
//从27个体素最里测最下面一行开始计算

//EdgeVertexIndex存储的边的顺序都是按x,y,z轴正向坐标存储且都是逆时针，0-1,4-5,7-6,3-2,0-3,1-2,5-6,4-7,0-4,3-7,2-6,1-5
//其中相交体素是通过给定的veg文件中的set求相交，但是经过SpatialVoxelExtraction获取降维后的Set后导致相交的数据量减少

//CubicVegMesh::CubicVegMesh(const std::string& vModelPath, bool vCalculateVoxelRelated)
//{
//    __loadVegMesh(vModelPath, vCalculateVoxelRelated);
//
//
//    m_NumRegions = m_SetRegionsRelatedData.size();
//    for (int i = 0; i < m_NumRegions; i++)
//        {
//            for (int k = i + 1; k < m_NumRegions; k++)
//            {
//                __SearchIntersectVoxelGroup(i, k);
//            }
//        }
//
//    m_NumIntersectRegions = m_DifferentRegionsIntersectVoxel.size();
//}

CubicVegMesh::CubicVegMesh(const std::string& vModelPath, const std::string& vOriginVegSetPath, bool vCalculateVoxelRelated, bool vUseOriginVegCalculateIntersectVoxel)
{
    m_FileDirectory = vModelPath.substr(0, vModelPath.find_last_of("/"));
    m_FileDirectory += "/";

    __loadVegMesh(vModelPath,vCalculateVoxelRelated);

    if (vUseOriginVegCalculateIntersectVoxel == true)
    {
        ReadBaseVegRegionVoxel(vOriginVegSetPath);
        __OriginRegionRelatedVegElement();
        m_NumRegions = m_OriginSetRegionsRelatedData.size();
        for (int i = 0; i < m_NumRegions; i++)
        {
            for (int k = i + 1; k < m_NumRegions; k++)
            {
                __OriginSearchIntersectVoxelGroup(i, k);
            }
        }
    }
    else
    {
        m_NumRegions = m_SetRegionsRelatedData.size();
        for (int i = 0; i < m_NumRegions; i++)
        {
            for (int k = i + 1; k < m_NumRegions; k++)
            {
                __SearchIntersectVoxelGroup(i, k);
            }
        }      
    }
    m_NumIntersectRegions = m_DifferentRegionsIntersectVoxel.size();

    __InitColor();
}

CubicVegMesh::CubicVegMesh(const CubicVegMesh & volumetricMesh)
{
   
}

void CubicVegMesh::ReadBaseVegRegionVoxel(const std::string &vPath)
{
    std::ifstream VegFile(vPath);
    std::string lineString;
    while (getline(VegFile, lineString))
    {
        std::vector<std::string> SETS;
        boost::split(SETS, lineString, boost::is_any_of(" "), boost::token_compress_off);
        if (SETS[0] == "*SET")
        {
            getline(VegFile, lineString);
            std::vector<std::string> RegionPath;
            boost::split(RegionPath, lineString, boost::is_any_of(" "), boost::token_compress_off);
            std::vector<int> ElementIndex = ReadFixedIndex(RegionPath[1]);
            m_OriginSetRegions.push_back(std::make_pair(SETS[1], ElementIndex));
        }        
    }
}

void CubicVegMesh::SetMassAndMaterialCalulacteValueRelated(double vmaxLinearPara, double vminLinearPara)
{
    m_MaterialLinearMaxPara = vmaxLinearPara;
    m_MaterialLinearMinPara = vminLinearPara;
    m_MaterialLinearMaxAndMinDiffPara = m_MaterialLinearMaxPara - m_MaterialLinearMinPara;
    m_MinMassPara = DBL_MAX;
    m_MinStiffnessMaterialPara = DBL_MAX;
    m_MaxStiffnessMaterialPara = 0;
    CENUMaterial *tempENU=nullptr;
    CORTHOTROPIC_N1Material *tempCORTHOTROPIC_N1=nullptr;
    for (int RegionsNumber = 0; RegionsNumber < m_SetRegionsRelatedData.size(); RegionsNumber++)
    {
        CMaterial* tempMaterial = __GetGroupSetRelatedMaterial(RegionsNumber);
        Common::EMaterialType tempMaterialType = tempMaterial->MaterialType;
        if (tempMaterialType == Common::EMaterialType::ENU)
        {
            tempENU = dynamic_cast<CENUMaterial*>(tempMaterial);
            if (m_MinMassPara > tempENU->Mass) m_MinMassPara = tempENU->Mass;
            //m_MinMassPara = std::min(m_MinMassPara, tempENU->Mass);
            if (m_MinStiffnessMaterialPara > tempENU->E) m_MinStiffnessMaterialPara = tempENU->E;
            if(m_MaxStiffnessMaterialPara< tempENU->E) m_MaxStiffnessMaterialPara= tempENU->E;
            //m_MinStiffnessMaterialPara = std::min(m_MinStiffnessMaterialPara, tempENU->E);
            //m_MaxStiffnessMaterialPara = std::max(m_MaxStiffnessMaterialPara, tempENU->E);
        }
        else if (tempMaterialType == Common::EMaterialType::ORTHOTROPIC_N1)
        {
            tempCORTHOTROPIC_N1 = dynamic_cast<CORTHOTROPIC_N1Material*>(tempMaterial);
            //m_MinMassPara = std::min(m_MinMassPara, tempCORTHOTROPIC_N1->Mass);
            if (m_MinMassPara > tempCORTHOTROPIC_N1->Mass) m_MinMassPara = tempCORTHOTROPIC_N1->Mass;
            if (m_MinStiffnessMaterialPara > tempCORTHOTROPIC_N1->E11) m_MinStiffnessMaterialPara = tempCORTHOTROPIC_N1->E11;
            if (m_MinStiffnessMaterialPara > tempCORTHOTROPIC_N1->E22) m_MinStiffnessMaterialPara = tempCORTHOTROPIC_N1->E22;
            if (m_MinStiffnessMaterialPara > tempCORTHOTROPIC_N1->E33) m_MinStiffnessMaterialPara = tempCORTHOTROPIC_N1->E33;

            if (m_MaxStiffnessMaterialPara < tempCORTHOTROPIC_N1->E11) m_MaxStiffnessMaterialPara = tempCORTHOTROPIC_N1->E11;
            if (m_MaxStiffnessMaterialPara < tempCORTHOTROPIC_N1->E22) m_MaxStiffnessMaterialPara = tempCORTHOTROPIC_N1->E22;
            if (m_MaxStiffnessMaterialPara < tempCORTHOTROPIC_N1->E33) m_MaxStiffnessMaterialPara = tempCORTHOTROPIC_N1->E33;

            //m_MinStiffnessMaterialPara = std::min(std::min(std::min(m_MinStiffnessMaterialPara, tempCORTHOTROPIC_N1->E11), tempCORTHOTROPIC_N1->E22), tempCORTHOTROPIC_N1->E33);
            //m_MaxStiffnessMaterialPara = std::max(std::max(std::max(m_MinStiffnessMaterialPara, tempCORTHOTROPIC_N1->E11), tempCORTHOTROPIC_N1->E22), tempCORTHOTROPIC_N1->E33);
        }
        
    }
    m_MaxAndMinMaterialDiffPara = m_MaxStiffnessMaterialPara - m_MinStiffnessMaterialPara;
}

void CubicVegMesh::InitVegRenderingProcess()
{
    __processVegMesh();

    __setupMesh();
    
}

void CubicVegMesh::InitVegRenderingLabeledVoxel()
{
    __processVegFixedElementsMesh();
    m_FixedEleVAO.resize(m_NumRegions + m_NumIntersectRegions);
    m_FixedEleVBO.resize(m_NumRegions + m_NumIntersectRegions);
    m_FixedEleEBO.resize(m_NumRegions + m_NumIntersectRegions);
    for(int i=0;i<m_NumRegions+m_NumIntersectRegions;i++)
    __setupFixedElementMesh(i);
}

void CubicVegMesh::SaveKeyStiffnessVoxel(const std::string & vDirectionPath)
{
    for (int RegionsNumber = 0; RegionsNumber < m_VoxelGroups.size(); RegionsNumber++)
    {
        std::string FilePath = vDirectionPath;
        FilePath += "/newTest";
        FilePath += std::to_string(RegionsNumber);
        FilePath += ".txt";
        ofstream fout(FilePath.c_str());
        for (int ChildRegionsIndex = 0; ChildRegionsIndex < m_VoxelGroups[RegionsNumber].size(); ChildRegionsIndex++)
        {
            for (int VoxelIndex = 0; VoxelIndex < m_VoxelGroups[RegionsNumber][ChildRegionsIndex].size(); VoxelIndex++)
            {
                fout << m_VoxelGroups[RegionsNumber][ChildRegionsIndex][VoxelIndex].first.ElementIndex<<",";
            }
        }
        fout.close();       
    }
}

//void CubicVegMesh::draw(const CShader& vShader) const
//{
//    //glBindVertexArray(m_FixedEleVAO);
//    //glDrawElements(GL_TRIANGLES, m_CubeIndiceStruct.size(), GL_UNSIGNED_INT, 0);
//    ////glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
//    //glBindVertexArray(0);
//}

void CubicVegMesh::DrawVegFiexedCubic(const CShader& vShader) const
{
    if (m_NumRegions == 0)
        std::cout << "read Rendering Veg File Error" << std::endl;
    for (int i = m_NumIntersectRegions + m_NumRegions; i >= 0; i--)
    {      
        if (i < m_NumRegions)
        {
            //continue;
            //vShader.setVec4("renderingColor", glm::vec4(0,0,0,1));
            vShader.setVec4("renderingColor", m_RenderingColor[i]);
        }
        else
        {
            vShader.setVec4("renderingColor", glm::vec4(0.501, 0.0, 0.501, 1));
        }
        
        glBindVertexArray(m_FixedEleVAO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_FixedEleEBO[i]);
        glDrawElements(GL_TRIANGLES, m_DifferentRegionsCubeIndiceStruct[i].size(), GL_UNSIGNED_INT, 0);
        //glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
        glBindVertexArray(0);
    }
    
}

void CubicVegMesh::DrawVegSpecificFixedCubic(const CShader& vShader, int vVoxelGroupIndex) const
{
    if (vVoxelGroupIndex > m_DifferentRegionsCubeIndiceStruct.size())
    {
        vVoxelGroupIndex = 0;
    }
    vShader.setVec4("renderingColor", m_RenderingColor[vVoxelGroupIndex]);
    glBindVertexArray(m_FixedEleVAO[vVoxelGroupIndex]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_FixedEleEBO[vVoxelGroupIndex]);
    glDrawElements(GL_TRIANGLES, m_DifferentRegionsCubeIndiceStruct[vVoxelGroupIndex].size(), GL_UNSIGNED_INT, 0);
    //glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
    glBindVertexArray(0);
}

void CubicVegMesh::DrawVegLine(const CShader& vShader) const
{
    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_LINES, m_LineIndiceStruct.size(), GL_UNSIGNED_INT, 0);
    //glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0, m_InstanceTreeNumber);
    glBindVertexArray(0);
}

//deal VEG Face
void CubicVegMesh::__processVegMesh()
{
    for (int i = 0; i < m_VegElements.size(); i++)
    {
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[0]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[1]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[1]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[2]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[2]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[3]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[3]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[0]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[4]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[5]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[5]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[6]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[6]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[7]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[7]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[4]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[0]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[4]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[1]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[5]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[2]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[6]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[3]);
        m_LineIndiceStruct.push_back(m_VegElements[i].VertexIndex[7]);
    }
}

void CubicVegMesh::__processVegFixedElementsMesh()
{
    m_DifferentRegionsCubeIndiceStruct.resize(m_NumRegions + m_NumIntersectRegions);
    //cull face needs to be counterclockwise
    for(int i=0;i<m_SetRegionsRelatedData.size();i++)
        for (int k = 0; k < m_SetRegionsRelatedData[i].second.size(); k++)
        {
            __pushbackVoxelFace(i, m_SetRegionsRelatedData[i].second[k]);           
        }
    for(int i = 0; i < m_NumIntersectRegions; i++)
        for (int k = 0; k < m_DifferentRegionsIntersectVoxel[i].size(); k++)
        {
            __pushbackVoxelFace(i+m_NumRegions, m_DifferentRegionsIntersectVoxel[i][k]);
        }
}

void CubicVegMesh::__pushbackVoxelFace(int vRegionsCubeStructIndex, Common::SVegElement & vVoxelElement)
{
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[0]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[3]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[1]);
                                       
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[1]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[3]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[2]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[4]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[5]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[7]);
                                       
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[5]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[6]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[7]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[0]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[1]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[4]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[1]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[5]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[4]);
                                       
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[2]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[3]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[7]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[6]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[2]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[7]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[0]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[4]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[3]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[4]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[7]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[3]);
                                      
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[1]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[2]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[5]);
                                       
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[5]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[2]);
    m_DifferentRegionsCubeIndiceStruct[vRegionsCubeStructIndex].push_back(vVoxelElement.VertexIndex[6]);
}

void CubicVegMesh::__setupMesh()
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
    glBufferData(GL_ARRAY_BUFFER, m_VegVertices.size() * sizeof(glm::vec3), &m_VegVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_LineIndiceStruct.size() * sizeof(unsigned int), &m_LineIndiceStruct[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void CubicVegMesh::__setupFixedElementMesh(int vRegionsIndex)
{
    glGenVertexArrays(1, &m_FixedEleVAO[vRegionsIndex]);
    glGenBuffers(1, &m_FixedEleVBO[vRegionsIndex]);
    glGenBuffers(1, &m_FixedEleEBO[vRegionsIndex]);

    glBindVertexArray(m_FixedEleVAO[vRegionsIndex]);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_FixedEleVBO[vRegionsIndex]);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    //m_Mesh.
    glBufferData(GL_ARRAY_BUFFER, m_VegVertices.size() * sizeof(glm::vec3), &m_VegVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_FixedEleEBO[vRegionsIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_DifferentRegionsCubeIndiceStruct[vRegionsIndex].size() * sizeof(unsigned int), &m_DifferentRegionsCubeIndiceStruct[vRegionsIndex][0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void CubicVegMesh::__loadVegMesh(const std::string& vModelPath,bool vCalculateVoxelRelated)
{
    std::ifstream VegFile(vModelPath);
    std::string lineString;
    while (getline(VegFile, lineString))
    {
        if (lineString == "*VERTICES")
        {
            getline(VegFile, lineString);
            std::vector<std::string> VERTICESNUM;
            boost::split(VERTICESNUM, lineString, boost::is_any_of(" "), boost::token_compress_off);
            m_CountNumberVertices=std::stoi(VERTICESNUM[0]);
            for (int i = 0; i < m_CountNumberVertices; i++)
            {
                getline(VegFile, lineString);
                std::vector<std::string> EachPoint;
                boost::split(EachPoint, lineString, boost::is_any_of(" "), boost::token_compress_off);
                m_VegVertices.push_back(glm::vec3(std::stof(EachPoint[1]), std::stof(EachPoint[2]), std::stof(EachPoint[3])));
            }
        }

        if (lineString == "*ELEMENTS")
        {
            getline(VegFile, lineString);
            getline(VegFile, lineString);
            std::vector<std::string> ELEMENTS;
            boost::split(ELEMENTS, lineString, boost::is_any_of(" "), boost::token_compress_off);
            m_CountNumElements = std::stoi(ELEMENTS[0]);
            for (int i = 0; i < m_CountNumElements; i++)
            {
                getline(VegFile, lineString);
                std::vector<std::string> EachElementLine;
                boost::split(EachElementLine, lineString, boost::is_any_of(" "), boost::token_compress_off);
                int ElementIndex = std::stoi(EachElementLine[0]);
                std::vector<int> tempEleIndex;
                for (int k = 1; k < 9; k++)
                {            
                    tempEleIndex.push_back(std::stoi(EachElementLine[k])-1);
                }
                m_VegElements.push_back(Common::SVegElement(ElementIndex, tempEleIndex));              
            }
        }

        std::vector<std::string> Material;
        boost::split(Material, lineString, boost::is_any_of(" "), boost::token_compress_off);
        if (Material[0] == "*MATERIAL")
        {
            getline(VegFile, lineString);
            std::vector<std::string> MaterialData;
            boost::split(MaterialData, lineString, boost::is_any_of(","), boost::token_compress_off);
            if (MaterialData[0] == "ENU")
            {
                CENUMaterial* temp=new CENUMaterial(atof(MaterialData[1].c_str()), atof(MaterialData[2].c_str()), atof(MaterialData[3].c_str()));
                m_Materials[Material[1]]= temp;

                CENUMaterial* test = dynamic_cast<CENUMaterial*>(m_Materials[Material[1]]);
            }
            else if(MaterialData[0]=="ORTHOTROPIC_N1")
            {
                CORTHOTROPIC_N1Material *temp = new CORTHOTROPIC_N1Material(atof(MaterialData[1].c_str()), atof(MaterialData[2].c_str()), atof(MaterialData[3].c_str()), atof(MaterialData[4].c_str()), atof(MaterialData[5].c_str()));
                m_Materials[Material[1]] = temp;
            }
        }

        std::vector<std::string> SETS;
        boost::split(SETS, lineString, boost::is_any_of(" "), boost::token_compress_off);
        if (SETS[0] == "*SET")
        {
            getline(VegFile, lineString);
            std::vector<std::string> RegionPath;
            boost::split(RegionPath, lineString, boost::is_any_of(" "), boost::token_compress_off);
            std::vector<int> ElementIndex = ReadFixedIndex(RegionPath[1]);
            m_SetRegions.push_back(std::make_pair(SETS[1], ElementIndex));
        }

        std::vector<std::string> REGION;   
        if (lineString == "*REGION")
        {       
            getline(VegFile, lineString);
            std::vector<std::string> RegionWithMaterial;
            boost::split(RegionWithMaterial, lineString, boost::is_any_of(", "), boost::token_compress_on);
            m_FileRegionWithMaterials[RegionWithMaterial[0]] = RegionWithMaterial[1];
        }
    }
    //前面的不同Set可能用相同的Material，以及不需要用到的Regions所以在存储后重新组织成与Set相同的REGION大小构成  

    if(vCalculateVoxelRelated==true)
    __CalculateVoxelRelatedVoxel();

    __RegionRelatedVegElement();
}

//void CubicVegMesh::__ResetRegionWithMaterialsDataSet()
//{
//    for (auto setRegion : m_SetRegions)
//    {
//        setRegion.first
//    }
//    m_ReSetRegionsWithMaterials
//}

#pragma region MainlyUseFirstButInRenderingUseSecondfunction

void CubicVegMesh::__RegionRelatedVegElement()
{
    for (int i = 0; i < m_SetRegions.size(); i++)
    {
        std::vector<Common::SVegElement> tempVegElement;
        for (int k = 0; k < m_SetRegions[i].second.size(); k++)
        {
            for (int ElementsIndex = 0; ElementsIndex < m_CountNumElements; ElementsIndex++)
            {
                if (m_VegElements[ElementsIndex].ElementIndex == m_SetRegions[i].second[k])
                {
                    tempVegElement.push_back(m_VegElements[ElementsIndex]);
                    break;
                }
            }
        }
        m_SetRegionsRelatedData.push_back(std::make_pair(m_SetRegions[i].first, tempVegElement));     
    }
}

void CubicVegMesh::__OriginRegionRelatedVegElement()
{
    for (int i = 0; i < m_OriginSetRegions.size(); i++)
    {
        std::vector<Common::SVegElement> tempVegElement;
        for (int k = 0; k < m_OriginSetRegions[i].second.size(); k++)
        {
            for (int ElementsIndex = 0; ElementsIndex < m_CountNumElements; ElementsIndex++)
            {
                if (m_VegElements[ElementsIndex].ElementIndex == m_OriginSetRegions[i].second[k])
                {
                    tempVegElement.push_back(m_VegElements[ElementsIndex]);
                    break;
                }
            }
        }
        m_OriginSetRegionsRelatedData.push_back(std::make_pair(m_OriginSetRegions[i].first, tempVegElement));
    }
}

#pragma endregion

#pragma region MainlyUseFirstButInRenderingUseSecondfunction

void CubicVegMesh::__SearchIntersectVoxelGroup(int vFirstRegionsIndex, int vSecondRegionsIndex)
{
    std::vector<Common::SVegElement> IntersectVoxels;
    for (auto FirstRegionsVoxel : m_SetRegionsRelatedData[vFirstRegionsIndex].second)
    {
        for (auto SecondRegionsVoxel : m_SetRegionsRelatedData[vSecondRegionsIndex].second)
        {
            if (FirstRegionsVoxel == SecondRegionsVoxel)
            {
                IntersectVoxels.push_back(FirstRegionsVoxel);

            }
        }
    }
    if (IntersectVoxels.size() != 0)
    {
        m_DifferentRegionsIntersectVoxel.push_back(IntersectVoxels);
    }   
}

void CubicVegMesh::__OriginSearchIntersectVoxelGroup(int vFirstRegionsIndex, int vSecondRegionsIndex)
{
    std::vector<Common::SVegElement> IntersectVoxels;
    for (auto FirstRegionsVoxel : m_OriginSetRegionsRelatedData[vFirstRegionsIndex].second)
    {
        for (auto SecondRegionsVoxel : m_OriginSetRegionsRelatedData[vSecondRegionsIndex].second)
        {
            if (FirstRegionsVoxel == SecondRegionsVoxel)
            {
                IntersectVoxels.push_back(FirstRegionsVoxel);

            }
        }
    }
    if (IntersectVoxels.size() != 0)
    {
        m_DifferentRegionsIntersectVoxel.push_back(IntersectVoxels);
    }
}

#pragma endregion

std::vector<int> CubicVegMesh::ReadFixedIndex(const std::string& vFilePath)
{
    std::vector<int> FixedIndex;
    std::string tempFilePath = m_FileDirectory + vFilePath;
    std::ifstream VegFile(tempFilePath);
    std::string lineString;
    while (getline(VegFile, lineString))
    {
        std::vector<std::string> FixedPoint;
        boost::split(FixedPoint, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = FixedPoint.begin(); it != FixedPoint.end(); ++it)
        {
            if (*it == "")
                break;
            FixedIndex.push_back(std::stoi(*it));
        }
    }
    return FixedIndex;
}

//Skeleton voxel grouping
//两种方案一种是用m_VoxelGroups来进行小的组的构建
//另一种是直接用不同的Groups来进行构建m_VoxelGroupWithEachRegions

void CubicVegMesh::ConstructVoxelGroup()
{
    m_VoxelGroups.resize(m_SetRegionsRelatedData.size());
    m_VoxelGroupWithEachRegions.resize(m_SetRegionsRelatedData.size());
    for (int RegionsNumber = 0; RegionsNumber < m_SetRegionsRelatedData.size(); RegionsNumber++)
    {
        int ElementNumber = m_SetRegionsRelatedData[RegionsNumber].second.size();
        int HasPushNumber = ElementNumber;
        std::vector<bool> IsPushInGroup(ElementNumber, false);
        while (HasPushNumber > 0)
        {
            std::vector<std::pair<Common::SVegElement,int>> tempGroupElements;
            std::set<int> tempGroupVerticesIndex;
            for (int i = 0; i < ElementNumber; i++)
            {
                if (IsPushInGroup[i] == true)
                    continue;
                Common::SVegElement temp = m_SetRegionsRelatedData[RegionsNumber].second[i];
                if (tempGroupElements.size() == 0 || __findVoxelVerticesinGroup(tempGroupVerticesIndex, RegionsNumber, i) == true)
                {
                    tempGroupElements.push_back(std::make_pair(temp,0));
                    for (int k = 0; k < temp.VertexIndex.size(); k++)
                    {
                        tempGroupVerticesIndex.insert(temp.VertexIndex[k]);
                    }
                    IsPushInGroup[i] = true;
                    HasPushNumber -= 1;
                }
            }
            m_VoxelGroups[RegionsNumber].push_back(tempGroupElements);
            m_GroupVertices.push_back(tempGroupVerticesIndex);
            tempGroupElements.clear();
            tempGroupVerticesIndex.clear();
        }
    }
   
}

void CubicVegMesh::__CalculateGroupVoxelValue()
{
    CENUMaterial *tempENU=nullptr;
    CORTHOTROPIC_N1Material *tempCORTHOTROPIC_N1=nullptr;
    for (int RegionsNumber = 0; RegionsNumber < m_VoxelGroups.size(); RegionsNumber++)
    {
       
        CMaterial* tempMaterial= __GetGroupSetRelatedMaterial(RegionsNumber);
        Common::EMaterialType tempMaterialType = tempMaterial->MaterialType;
        if (tempMaterialType == Common::EMaterialType::ENU)
        {
            tempENU= dynamic_cast<CENUMaterial*>(tempMaterial);
        }
        else if(tempMaterialType == Common::EMaterialType::ORTHOTROPIC_N1)
        {
            tempCORTHOTROPIC_N1 =dynamic_cast<CORTHOTROPIC_N1Material*>(tempMaterial);
        }

        std::vector<std::pair<Common::SVegElement, int>> tempGroupsVoxel;
        
        for (int ChildRegionsIndex = 0; ChildRegionsIndex < m_VoxelGroups[RegionsNumber].size(); ChildRegionsIndex++)
        {
            if (tempMaterialType == Common::EMaterialType::ENU)
            {
                __calculateChildGroupsValue(RegionsNumber, ChildRegionsIndex, *tempENU);
            }
            else if(tempMaterialType== Common::EMaterialType::ORTHOTROPIC_N1)
            {
                __calculateChildGroupsValue(RegionsNumber, ChildRegionsIndex, *tempCORTHOTROPIC_N1);
            }                         
        }
               
    }

}

void CubicVegMesh::EraseMaxValueVoxelWithAllChildGroup(int vIndexRegionNumber)
{
    if (vIndexRegionNumber < m_VoxelGroups.size())
    {
        __CalculateGroupVoxelValue();

        std::vector<Common::SSubgGroupMaxValVoxel> GroupMaxValVoxeles;
        for (int ChildRegionsIndex = 0; ChildRegionsIndex < m_VoxelGroups[vIndexRegionNumber].size(); ChildRegionsIndex++)
        {
            __sortMaximumValueVoxels(m_VoxelGroups[vIndexRegionNumber][ChildRegionsIndex]);

            if (m_VoxelGroups[vIndexRegionNumber][ChildRegionsIndex].size() != 0)
            {
                GroupMaxValVoxeles.push_back(Common::SSubgGroupMaxValVoxel(ChildRegionsIndex, m_VoxelGroups[vIndexRegionNumber][ChildRegionsIndex][0].second));
            }
        }      
        //std::cout << "ChildIndex" << ChildIndex << std::endl;

        int EraseChildIndex = __SubgroupRandomSameValueVoxelIndex(GroupMaxValVoxeles);

        //std::cout << "ChildIndex" << EraseChildIndex << std::endl;
        __eraseMaximumValueVoxels(m_VoxelGroups[vIndexRegionNumber][EraseChildIndex]);
    }
    else
    {
        std::cout << "RegionsIndex Error" << std::endl;
    }  
}

//void CubicVegMesh::EraseRandomVoxel()
//{
//    int RandomRegionGroup = GetRandomNumber(0,m_VoxelGroups.size()-1);
//    int RandomSubRegionsGroup = GetRandomNumber(0, m_VoxelGroups[RandomRegionGroup].size() - 1);
//
//    bool tempJudge = __eraseMaximumValueVoxels(m_VoxelGroups[RandomRegionGroup][RandomSubRegionsGroup]);
//    if (tempJudge == false)
//    {
//        EraseRandomVoxel();
//    }
//}

void CubicVegMesh::EraseSpecGroupRandomVoxel(int vRegionGroup)
{
    int RandomSubRegionsGroup = RandomNumber(0, m_VoxelGroups[vRegionGroup].size() - 1);

    bool tempJudge = __eraseMaximumValueVoxels(m_VoxelGroups[vRegionGroup][RandomSubRegionsGroup]);
    if (tempJudge == false)
    {
        EraseSpecGroupRandomVoxel(vRegionGroup);
    }
}

//重写几种类别的输入材质,如果再这个函数转换类型，会产生多次dynamic转换

#pragma region __calculateChildGroupsValue
void CubicVegMesh::__calculateChildGroupsValue(int vRegionsNumber, int vChildRegionsIndex)
{
    for (auto& Voxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
    {
        for (auto &SeconVoxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
        {
            for (int i = 0; i < Voxel.first.FaceRelatedIndex.size(); i++)
            {
                //Voxel.second = 4;
                if (Voxel.first.FaceRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {
                    
                    Voxel.second += 4;
                }
            }

            for (int i = 0; i < Voxel.first.EdgeRelatedIndex.size(); i++)
            {
                if (Voxel.first.EdgeRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {
                    Voxel.second += 1;
                }
            }
        }      
    }
}

void CubicVegMesh::__calculateChildGroupsValue(int vRegionsNumber, int vChildRegionsIndex, CENUMaterial vMaterial)
{
    for (auto& Voxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
    {
        Voxel.second = 0;
        for (auto &SeconVoxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
        {
            for (int i = 0; i < Voxel.first.FaceRelatedIndex.size(); i++)
            {
                //Voxel.second = 4;
                if (Voxel.first.FaceRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {

                    Voxel.second += 4;
                }
            }

            for (int i = 0; i < Voxel.first.EdgeRelatedIndex.size(); i++)
            {
                if (Voxel.first.EdgeRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {
                    Voxel.second += 1;
                }
            }
        }

        Voxel.second += vMaterial.Mass / m_MinMassPara;
        Voxel.second += __MaterialKValue(vMaterial.E);
    }
}

void CubicVegMesh::__calculateChildGroupsValue(int vRegionsNumber, int vChildRegionsIndex, CORTHOTROPIC_N1Material vMaterial)
{
    for (auto& Voxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
    {
        Voxel.second = 0;
        for (auto &SeconVoxel : m_VoxelGroups[vRegionsNumber][vChildRegionsIndex])
        {
            for (int i = 0; i < Voxel.first.FaceRelatedIndex.size(); i++)
            {
                //Voxel.second = 4;
                if (Voxel.first.FaceRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {

                    Voxel.second += 4;
                }
            }

            for (int i = 0; i < Voxel.first.EdgeRelatedIndex.size(); i++)
            {
                if (Voxel.first.EdgeRelatedIndex[i] == SeconVoxel.first.ElementIndex)
                {
                    Voxel.second += 1;
                }
            }
        }

        Voxel.second += vMaterial.Mass / m_MinMassPara;
        Voxel.second += 1/3*(__MaterialKValue(vMaterial.E11)+ __MaterialKValue(vMaterial.E22)+ __MaterialKValue(vMaterial.E33));
    }
}
#pragma endregion

void CubicVegMesh::__sortMaximumValueVoxels(std::vector<std::pair<Common::SVegElement, int>> &vChildGroup)
{
    std::sort(vChildGroup.begin(), vChildGroup.end(), __compVoxelValue);
}

bool CubicVegMesh::__eraseMaximumValueVoxels(std::vector<std::pair<Common::SVegElement, int>> &vChildGroup)
{    
    if (vChildGroup.size() > 0)
    {
        std::vector<std::pair<Common::SVegElement, int>>::iterator it = vChildGroup.begin();
        vChildGroup.erase(it);
        return true;
    }
    return false;
}

bool CubicVegMesh::__findVoxelVerticesinGroup(std::set<int> & GroupVerticesIndex, int RegionsIndex, int ElementIndex)
{
    
    for (auto it : GroupVerticesIndex)
    {
        for (int k = 0; k < 8; k++)
        {
            if (it == m_SetRegionsRelatedData[RegionsIndex].second[ElementIndex].VertexIndex[k])
            {
                return true;
            }
        }  
    }
    return false;
}

void CubicVegMesh::__CalculateVoxelRelatedVoxel()
{
    __calculateVoxelEdge();
    for (int i = 0; i < m_CountNumElements; i++)
    {
        int start = i - 300;
        if (start < 0)
            start = 0;
        int end = i + 300;
        if (end > m_CountNumElements)
            end = m_CountNumElements;
        for (int k = start; k < end; k++)
        {
            //calculate RealteFace backface
            if (m_VegElements[i].EdgeVertexIndex[0] == m_VegElements[k].EdgeVertexIndex[1]&&
                m_VegElements[i].EdgeVertexIndex[3]== m_VegElements[k].EdgeVertexIndex[2] &&
                m_VegElements[i].EdgeVertexIndex[4] == m_VegElements[k].EdgeVertexIndex[7] &&
                m_VegElements[i].EdgeVertexIndex[5] == m_VegElements[k].EdgeVertexIndex[6])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }

            //frontface
            if (m_VegElements[i].EdgeVertexIndex[1] == m_VegElements[k].EdgeVertexIndex[0] &&
                m_VegElements[i].EdgeVertexIndex[2] == m_VegElements[k].EdgeVertexIndex[3] &&
                m_VegElements[i].EdgeVertexIndex[7] == m_VegElements[k].EdgeVertexIndex[4] &&
                m_VegElements[i].EdgeVertexIndex[6] == m_VegElements[k].EdgeVertexIndex[5])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }

            //leftface
            if (m_VegElements[i].EdgeVertexIndex[8] == m_VegElements[k].EdgeVertexIndex[11] &&
                m_VegElements[i].EdgeVertexIndex[9] == m_VegElements[k].EdgeVertexIndex[10] &&
                m_VegElements[i].EdgeVertexIndex[4] == m_VegElements[k].EdgeVertexIndex[5] &&
                m_VegElements[i].EdgeVertexIndex[7] == m_VegElements[k].EdgeVertexIndex[6])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }
            //rightface
            if (m_VegElements[i].EdgeVertexIndex[11] == m_VegElements[k].EdgeVertexIndex[8] &&
                m_VegElements[i].EdgeVertexIndex[10] == m_VegElements[k].EdgeVertexIndex[9] &&
                m_VegElements[i].EdgeVertexIndex[5] == m_VegElements[k].EdgeVertexIndex[4] &&
                m_VegElements[i].EdgeVertexIndex[6] == m_VegElements[k].EdgeVertexIndex[7])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }
            //botface
            if (m_VegElements[i].EdgeVertexIndex[0] == m_VegElements[k].EdgeVertexIndex[3] &&
                m_VegElements[i].EdgeVertexIndex[1] == m_VegElements[k].EdgeVertexIndex[2] &&
                m_VegElements[i].EdgeVertexIndex[8] == m_VegElements[k].EdgeVertexIndex[9] &&
                m_VegElements[i].EdgeVertexIndex[11] == m_VegElements[k].EdgeVertexIndex[10])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }
            //topface
            if (m_VegElements[i].EdgeVertexIndex[3] == m_VegElements[k].EdgeVertexIndex[0] &&
                m_VegElements[i].EdgeVertexIndex[2] == m_VegElements[k].EdgeVertexIndex[1] &&
                m_VegElements[i].EdgeVertexIndex[9] == m_VegElements[k].EdgeVertexIndex[8] &&
                m_VegElements[i].EdgeVertexIndex[10] == m_VegElements[k].EdgeVertexIndex[11])
            {
                m_VegElements[i].FaceRelatedIndex.push_back(k+1);
                continue;
            }

            //x axis edge
            //0-1edge with 7-6 edge
            if (m_VegElements[i].EdgeVertexIndex[0] == m_VegElements[k].EdgeVertexIndex[2])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //4-5edge with 3-2 edge
            if (m_VegElements[i].EdgeVertexIndex[1] == m_VegElements[k].EdgeVertexIndex[3])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //7-6edge with 0-1 edge
            if (m_VegElements[i].EdgeVertexIndex[2] == m_VegElements[k].EdgeVertexIndex[0])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //3-2edge with 4-5edge
            if (m_VegElements[i].EdgeVertexIndex[3] == m_VegElements[k].EdgeVertexIndex[1])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //y axis edge
            //0-3edge with 5-6 edge
            if (m_VegElements[i].EdgeVertexIndex[4] == m_VegElements[k].EdgeVertexIndex[6])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //1-2edge with 4-7 edge
            if (m_VegElements[i].EdgeVertexIndex[5] == m_VegElements[k].EdgeVertexIndex[7])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //5-6edge with 0-3 edge
            if (m_VegElements[i].EdgeVertexIndex[6] == m_VegElements[k].EdgeVertexIndex[4])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //4-7edge with 1-2 edge
            if (m_VegElements[i].EdgeVertexIndex[7] == m_VegElements[k].EdgeVertexIndex[5])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //z axis edge
            //0-4edge with 2-6 edge
            if (m_VegElements[i].EdgeVertexIndex[8] == m_VegElements[k].EdgeVertexIndex[10])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //3-7edge with 1-5 edge
            if (m_VegElements[i].EdgeVertexIndex[9] == m_VegElements[k].EdgeVertexIndex[11])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //2-6edge with 0-4 edge
            if (m_VegElements[i].EdgeVertexIndex[10] == m_VegElements[k].EdgeVertexIndex[8])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }
            //1-5edge with 3-7 edge
            if (m_VegElements[i].EdgeVertexIndex[11] == m_VegElements[k].EdgeVertexIndex[9])
            {
                m_VegElements[i].EdgeRelatedIndex.push_back(k+1);
                continue;
            }

            if (m_VegElements[i].EdgeRelatedIndex.size() + m_VegElements[i].FaceRelatedIndex.size() == 18)
                break;

        }
    }

}

void CubicVegMesh::__calculateVoxelEdge()
{
    for (int i = 0; i < m_CountNumElements; i++)
    {
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[0], m_VegElements[i].VertexIndex[1]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[4], m_VegElements[i].VertexIndex[5]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[7], m_VegElements[i].VertexIndex[6]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[3], m_VegElements[i].VertexIndex[2]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[0], m_VegElements[i].VertexIndex[3]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[1], m_VegElements[i].VertexIndex[2]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[5], m_VegElements[i].VertexIndex[6]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[4], m_VegElements[i].VertexIndex[7]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[0], m_VegElements[i].VertexIndex[4]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[3], m_VegElements[i].VertexIndex[7]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[2], m_VegElements[i].VertexIndex[6]));
        m_VegElements[i].EdgeVertexIndex.push_back(Common::SLine(m_VegElements[i].VertexIndex[1], m_VegElements[i].VertexIndex[5]));
    }
}

int CubicVegMesh::__SubgroupRandomSameValueVoxelIndex(std::vector<Common::SSubgGroupMaxValVoxel>& GroupMaxValVoxeles)
{
    std::sort(GroupMaxValVoxeles.begin(), GroupMaxValVoxeles.end(), __compSubgroupVoxelValue);
    int tempMaxValue = GroupMaxValVoxeles[0].Value;
    std::vector<int> MaxValueGroupIndex;
    for (int i = 0; i < GroupMaxValVoxeles.size(); i++)
    {
        if (tempMaxValue == GroupMaxValVoxeles[i].Value)
            MaxValueGroupIndex.push_back(GroupMaxValVoxeles[i].ChildIndex);
    }
    std::default_random_engine e;
    LARGE_INTEGER seed;
    QueryPerformanceFrequency(&seed);
    QueryPerformanceCounter(&seed);
    e.seed(seed.QuadPart);
    std::uniform_int_distribution<unsigned> u(0, MaxValueGroupIndex.size() - 1);
    int RandomNumber = u(e);
    std::cout << "Same Value Number: " << MaxValueGroupIndex.size() << " RandomValueGroupIndex: " << MaxValueGroupIndex[RandomNumber]<<std::endl;
    return MaxValueGroupIndex[RandomNumber];




}

std::vector<int>& CubicVegMesh::GetAfterEraseRegionVoxelNumber()
{
    for (int RegionsIndex = 0; RegionsIndex < m_VoxelGroups.size(); RegionsIndex++)
    {
        int RegionsVoxelNumber = 0;
        for (int ChildRegionsIndex = 0; ChildRegionsIndex < m_VoxelGroups[RegionsIndex].size(); ChildRegionsIndex++)
        {
            RegionsVoxelNumber += m_VoxelGroups[RegionsIndex][ChildRegionsIndex].size();
        }
        m_AfterEraseVoxelNumber.push_back(RegionsVoxelNumber);
    }
    return m_AfterEraseVoxelNumber;
}

bool CubicVegMesh::__compVoxelValue(std::pair<Common::SVegElement, int> &vFirst, std::pair<Common::SVegElement, int>& vSecond)
{
    return vFirst.second > vSecond.second;
}

bool CubicVegMesh::__compSubgroupVoxelValue(Common::SSubgGroupMaxValVoxel& vFirst, Common::SSubgGroupMaxValVoxel& vSecond)
{
    return vFirst.Value > vSecond.Value;
}

CMaterial* CubicVegMesh::__GetGroupSetRelatedMaterial(int vGroupIndex)
{
    std::string SETName= m_SetRegionsRelatedData[vGroupIndex].first;
    std::string MaterialName= m_FileRegionWithMaterials[SETName];
    return m_Materials[MaterialName];
}

double CubicVegMesh::__MaterialKValue(double vMaterialK)
{
    return m_MaterialLinearMaxAndMinDiffPara / m_MaxAndMinMaterialDiffPara * vMaterialK + (m_MaterialLinearMinPara* m_MaxStiffnessMaterialPara - m_MaterialLinearMaxPara * m_MinStiffnessMaterialPara) / m_MaxAndMinMaterialDiffPara;
}

void CubicVegMesh::__InitColor()
{
    m_RenderingColor.resize(10);
    //different model should change
    m_RenderingColor[0] = glm::vec4(0.73, 0, 0, 1);
    m_RenderingColor[1] = glm::vec4(0.278, 0.73, 0, 1);
    m_RenderingColor[2] = glm::vec4(0, 0.474, 1, 1);
    m_RenderingColor[3] = glm::vec4(1, 0.6, 0.2, 1);
    m_RenderingColor[4] = glm::vec4(0, 0, 0.6, 1);
    m_RenderingColor[5] = glm::vec4(1, 0.2, 0.8, 1);
    m_RenderingColor[6] = glm::vec4(0, 0.2, 0, 1);
    m_RenderingColor[7] = glm::vec4(0, 0.5, 0.5, 1);
    m_RenderingColor[8] = glm::vec4(0.6, 0.2, 0, 1);
    m_RenderingColor[9] = glm::vec4(0.2, 0.8, 1, 1);
    
}

int CubicVegMesh::RandomNumber(int vMinRange, int vMaxRange)
{
    std::default_random_engine e;
    LARGE_INTEGER seed;
    QueryPerformanceFrequency(&seed);
    QueryPerformanceCounter(&seed);
    e.seed(seed.QuadPart);
    std::uniform_int_distribution<unsigned> u(vMinRange, vMaxRange);
    int Random = u(e);
    return Random;
}
