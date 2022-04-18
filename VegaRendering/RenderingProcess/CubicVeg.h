#pragma once
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Mesh.h"
#include "../Common/common.h"
#include "../Common/Material.h"
#include "../Common/ExtraTool.h"
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
#include <random>

class CubicVegMesh:public CBaseMesh
{
public:

    // Note: This class is abstract and cannot be instantiated; use the constructors in the derived classes (TetMesh, CubicMesh) to initialize a mesh, or use the load routine in volumetricMeshLoader.h
    CubicVegMesh() = default;

    //in RenderingProgress vCalculateVoxelRelated set false, and vUseOriginVegCalculateIntersectVoxel set true

    //CubicVegMesh(const std::string& vModelPath, bool vCalculateVoxelRelated);

    CubicVegMesh(const std::string& vModelPath, const std::string& vOriginVegSetPath,bool vCalculateVoxelRelated, bool vUseOriginVegCalculateIntersectVoxel);
    // copy constructor, destructor
    CubicVegMesh(const CubicVegMesh & volumetricMesh);
    //virtual CubicVegMesh * clone() = 0;
    ~CubicVegMesh()=default;


    void InitVegRenderingProcess();
    void InitVegRenderingLabeledVoxel();
    void SetMassAndMaterialCalulacteValueRelated(double vmaxLinearPara, double vminLinearPara);
    void ConstructVoxelGroup();
    std::vector<int>& GetAfterEraseRegionVoxelNumber();
    void EraseMaxValueVoxelWithAllChildGroup(int vIndexRegionNumber);

    //剔除模型的随机体素

    //void EraseRandomVoxel();
    void EraseSpecGroupRandomVoxel(int vRegionGroup);

    void SaveKeyStiffnessVoxel(const std::string & vDirectionPath);
    void ReadBaseVegRegionVoxel(const std::string &vPath);
    std::vector<int> ReadFixedIndex(const std::string& vFilePath);
    void draw(const CShader& vShader) const {};
    void DrawVegFiexedCubic(const CShader& vShader) const;
    void DrawVegLine(const CShader& vShader) const;
    void DrawVegSpecificFixedCubic(const CShader& vShader,int vVoxelGroupIndex) const;

protected:

private:
    void __loadVegMesh(const std::string& vModelPath, bool vCalculateVoxelRelated);
    void __processVegMesh();
    void __processVegFixedElementsMesh();
    void __setupMesh();
    void __setupFixedElementMesh(int vRegionsIndex);
    void __RegionRelatedVegElement();
    void __OriginRegionRelatedVegElement();
    void __CalculateVoxelRelatedVoxel();
    void __SearchIntersectVoxelGroup(int vFirstRegionsIndex,int vSecondRegionsIndex);
    void __OriginSearchIntersectVoxelGroup(int vFirstRegionsIndex, int vSecondRegionsIndex);
    bool __findVoxelVerticesinGroup(std::set<int> & vGroupVerticesIndex,int RegionsIndex,int ElementIndex);
    void __calculateVoxelEdge();
    void __sortMaximumValueVoxels(std::vector<std::pair<Common::SVegElement, int>> & vChildGroup);
    bool __eraseMaximumValueVoxels(std::vector<std::pair<Common::SVegElement, int>> & vChildGroup);
    int __SubgroupRandomSameValueVoxelIndex(std::vector<Common::SSubgGroupMaxValVoxel>& GroupMaxValVoxeles);
    void __calculateChildGroupsValue(int vRegionsNumber,int vChildRegionsIndex);
    void __calculateChildGroupsValue(int vRegionsNumber, int vChildRegionsIndex, CENUMaterial vMaterial);
    void __calculateChildGroupsValue(int vRegionsNumber, int vChildRegionsIndex, CORTHOTROPIC_N1Material vMaterial);
    void __pushbackVoxelFace(int vRegionsCubeStructIndex,Common::SVegElement & vVoxelElement);
    static bool __compVoxelValue(std::pair<Common::SVegElement, int>& vFirst, std::pair<Common::SVegElement, int>& vSecond);
    static bool __compSubgroupVoxelValue(Common::SSubgGroupMaxValVoxel& vFirst, Common::SSubgGroupMaxValVoxel& vSecond);
    void __CalculateGroupVoxelValue();
    void __InitColor();

    int RandomNumber(int vMinRange,int vMaxRange);

    //void __ResetRegionWithMaterialsDataSet();
    CMaterial* __GetGroupSetRelatedMaterial(int vGroupIndex);

    double __MaterialKValue(double vMaterialK);

    std::string m_FileDirectory;

    std::vector<unsigned int> m_FixedEleVAO;
    std::vector<unsigned int> m_FixedEleVBO;
    std::vector<unsigned int> m_FixedEleEBO;

    int m_CountNumberVertices=0;
    int m_CountNumElements = 0;
    int m_NumSets = 1;
    int m_NumRegions = 0;
    int m_NumMaterials = 0;

    int m_NumIntersectRegions = 0;

    //存储所有可能材质,第一个参数是材质名称
    std::map<std::string, CMaterial*> m_Materials;

    //每个REGION对应的材质名称第一个参数是Regions的SET名称，第二个参数是Materials的材质名称

    std::map<std::string, std::string> m_FileRegionWithMaterials;
    //m_SetRegions存储了SET文件中的体素索引
    std::vector < std::pair<std::string, std::vector<int>>> m_SetRegions;

    //每个Regions需要对应的刚度数据，当前的代码组织结构是错的，但不想改了，大概用数组去对应
    //std::vector<>
    //m_SetRegionsRelatedData存储了SET文件中体素索引的相关数据，不仅仅是索引

    std::vector<std::pair<std::string, std::vector<Common::SVegElement>>> m_SetRegionsRelatedData;
    std::vector<std::vector<Common::SVegElement>> m_DifferentRegionsIntersectVoxel;

    //第一个vector将模型分为不同的Groups用红绿蓝表示，第二个vector每个Group通过不连接部分进行部分成小的Group，
    //第三个vector每个小group中存在的多少个体素。
       
    std::vector<std::vector<std::vector<std::pair<Common::SVegElement,int>>>> m_VoxelGroups;

    //第一个vector将每个模型分为不同的Groups用红绿蓝表示，第二个为每个Group存在多少的体素。与前面不同的是前面分的更细，为了更好排序就重新拷贝了一边。

    std::vector<std::vector<std::pair<Common::SVegElement, int>>> m_VoxelGroupWithEachRegions;
    std::vector<std::set<int>> m_GroupVertices;
    std::vector<glm::vec3> m_VegVertices;
    std::vector<Common::SVegElement> m_VegElements;
    std::vector<int> m_LineIndiceStruct;
    std::vector<std::vector<int>> m_DifferentRegionsCubeIndiceStruct;

    std::vector<int> m_AfterEraseVoxelNumber;

    std::vector<glm::vec4> m_RenderingColor;

    double m_MaterialLinearMaxPara;
    double m_MaterialLinearMinPara;
    double m_MaterialLinearMaxAndMinDiffPara;
    double m_MinMassPara;
    double m_MaxStiffnessMaterialPara;
    double m_MinStiffnessMaterialPara;
    double m_MaxAndMinMaterialDiffPara;

    //m_SetRegions存储了原始Origin文件中的体素索引，因为原先的m_SetRegions可以表示经过体素提取后的

    std::vector < std::pair<std::string, std::vector<int>>> m_OriginSetRegions;
    std::vector<std::pair<std::string, std::vector<Common::SVegElement>>> m_OriginSetRegionsRelatedData;
};

