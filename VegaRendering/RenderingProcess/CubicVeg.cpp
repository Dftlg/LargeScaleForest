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

CubicVegMesh::CubicVegMesh(const std::string& vModelPath)
{
    __loadVegMesh(vModelPath);
    __processVegMesh();
    __processVegFixedElementsMesh();
    __setupMesh();
    __setupFixedElementMesh();
}

CubicVegMesh::CubicVegMesh(const CubicVegMesh & volumetricMesh)
{
   
}

void CubicVegMesh::draw(const CShader& vShader) const
{
    glBindVertexArray(m_FixedEleVAO);
    glDrawElements(GL_TRIANGLES, m_CubeIndiceStruct.size(), GL_UNSIGNED_INT, 0);
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
    //cull face needs to be counterclockwise
    //first version not consider different region
    for(int i=0;i<m_SetRegionsRelatedData.size();i++)
        for (int k = 0; k < m_SetRegionsRelatedData[i].second.size(); k++)
        {
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[0]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[3]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[1]);
                                        
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[1]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[3]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[2]);
                                         
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[4]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[5]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[7]);
                                        
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[5]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[6]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[7]);
                                       
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[0]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[1]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[4]);
                                        
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[1]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[5]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[4]);
                                         
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[2]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[3]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[7]);
                                         
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[6]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[2]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[7]);
                                        
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[0]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[4]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[3]);
                                    
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[4]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[7]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[3]);
                                  
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[1]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[2]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[5]);
                                         
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[5]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[2]);
            m_CubeIndiceStruct.push_back(m_SetRegionsRelatedData[i].second[k].VertexIndex[6]);
        }
   
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

void CubicVegMesh::__setupFixedElementMesh()
{
    glGenVertexArrays(1, &m_FixedEleVAO);
    glGenBuffers(1, &m_FixedEleVBO);
    glGenBuffers(1, &m_FixedEleEBO);

    glBindVertexArray(m_FixedEleVAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_FixedEleVBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    //m_Mesh.
    glBufferData(GL_ARRAY_BUFFER, m_VegVertices.size() * sizeof(glm::vec3), &m_VegVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_FixedEleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_CubeIndiceStruct.size() * sizeof(unsigned int), &m_CubeIndiceStruct[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void CubicVegMesh::__loadVegMesh(const std::string& vModelPath)
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
    }
    __RegionRelatedVegElement();
}

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

std::vector<int> CubicVegMesh::ReadFixedIndex(const std::string& vFilePath)
{
    std::vector<int> FixedIndex;
    std::ifstream VegFile(vFilePath);
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
