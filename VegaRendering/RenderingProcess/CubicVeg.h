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
#include "ComputerShader.h"
#include "BaseMesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <string>
#include <map>
#include "../../libraries/volumetricMesh/volumetricMeshParser.h"
#include "minivector.h"
#include "boundingBox.h"

class CubicVegMesh:public CBaseMesh
{
public:

    // Note: This class is abstract and cannot be instantiated; use the constructors in the derived classes (TetMesh, CubicMesh) to initialize a mesh, or use the load routine in volumetricMeshLoader.h
    CubicVegMesh() = default;
    CubicVegMesh(const std::string& vModelPath);
    // copy constructor, destructor
    CubicVegMesh(const CubicVegMesh & volumetricMesh);
    //virtual CubicVegMesh * clone() = 0;
    ~CubicVegMesh()=default;


    std::vector<int> ReadFixedIndex(const std::string& vFilePath);
    void draw(const CShader& vShader) const;
    void DrawVegLine(const CShader& vShader) const;

protected:

private:
    void __loadVegMesh(const std::string& vModelPath);
    void __processVegMesh();
    void __processVegFixedElementsMesh();
    void __setupMesh();
    void __setupFixedElementMesh();
    void __RegionRelatedVegElement();

    unsigned int m_FixedEleVAO;
    unsigned int m_FixedEleVBO;
    unsigned int m_FixedEleEBO;

    int m_CountNumberVertices=0;
    int m_CountNumElements = 0;
    int m_NumSets = 1;
    int m_NumRegions = 0;
    int m_NumMaterials = 0;

    std::vector < std::pair<std::string, std::vector<int>>> m_SetRegions;
    std::vector<std::pair<std::string, std::vector<Common::SVegElement>>> m_SetRegionsRelatedData;
    std::vector<glm::vec3> m_VegVertices;
    std::vector<Common::SVegElement> m_VegElements;

    std::vector<int> m_LineIndiceStruct;
    std::vector<int> m_CubeIndiceStruct;

};

