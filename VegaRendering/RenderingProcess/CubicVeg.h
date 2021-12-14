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


    void InitVegRenderingProcess();
    void InitVegRenderingLabeledVoxel();
    void ConstructVoxelGroup();
    void CalculateGroupVoxelValue();
    std::vector<int> ReadFixedIndex(const std::string& vFilePath);
    void draw(const CShader& vShader) const {};
    void DrawVegFiexedCubic(const CShader& vShader) const;
    void DrawVegLine(const CShader& vShader) const;

protected:

private:
    void __loadVegMesh(const std::string& vModelPath);
    void __processVegMesh();
    void __processVegFixedElementsMesh();
    void __setupMesh();
    void __setupFixedElementMesh(int vRegionsIndex);
    void __RegionRelatedVegElement();
    void __SearchIntersectVoxelGroup(int vFirstRegionsIndex,int vSecondRegionsIndex);
    bool __findVoxelVerticesinGroup(std::set<int> & vGroupVerticesIndex,int ElementIndex);
    void __calculateGroupEdge();
    void __pushbackVoxelFace(int vRegionsCubeStructIndex,Common::SVegElement & vVoxelElement);

    std::vector<unsigned int> m_FixedEleVAO;
    std::vector<unsigned int> m_FixedEleVBO;
    std::vector<unsigned int> m_FixedEleEBO;

    int m_CountNumberVertices=0;
    int m_CountNumElements = 0;
    int m_NumSets = 1;
    int m_NumRegions = 0;
    int m_NumMaterials = 0;

    int m_NumIntersectRegions = 0;

    std::vector < std::pair<std::string, std::vector<int>>> m_SetRegions;
    std::vector<std::pair<std::string, std::vector<Common::SVegElement>>> m_SetRegionsRelatedData;
    std::vector<std::vector<Common::SVegElement>> m_DifferentRegionsIntersectVoxel;
    std::vector<std::vector<Common::SVegElement>> m_VoxelGroup;
    std::vector<std::set<int>> m_GroupVertices;
    std::vector<glm::vec3> m_VegVertices;
    std::vector<Common::SVegElement> m_VegElements;
    std::vector<int> m_LineIndiceStruct;
    std::vector<std::vector<int>> m_DifferentRegionsCubeIndiceStruct;

};
