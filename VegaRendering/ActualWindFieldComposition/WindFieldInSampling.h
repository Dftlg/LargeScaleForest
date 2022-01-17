#pragma once
#include "stdafx.h"
#include <fstream>
#include <iostream>
#include<set>
#include"triangle.h"
#include "objMesh.h"
#include "triple.h"
#include "boundingBox.h"
#include "sceneObjectDeformable.h"
#include "ExtraTool.h"
#include <glm/glm.hpp>
#include "common.h"

//主要计算定向风源风源模型参看i3d 2017 Responsive Real-Time  Grass Rendering for General 3D Scenes
//在采样过程中0，0点处于左上，而渲染过程中处于左下

class CWindFieldSampling
{
public:
    CWindFieldSampling() = default;
    CWindFieldSampling(const ObjMesh * objMesh, const int resolution[3], int depth = 0, double expansionFactor = 1.2,double vTimeStep=1/60);
    // boxbmin, boxbmax, explicitly provide the bounding box
    CWindFieldSampling(const ObjMesh * objMesh, const int resolution[3], int depth, Vec3d boxbmin, Vec3d boxbmax);

    void SaveCorrespondingVoxelOfTreeGroup(const std::string & vDirectionPath);

    void GenerateWindFieldExternalForce(SceneObjectDeformable *& vtempSceneObjectDeformable,double * vof_ext);

    void GenerateWindFieldMeshAndWindDirectionLine(const std::string & vFilePath, const std::string & vExternFileDirectory);

    typedef triple<int, int, int> voxel;
    typedef triple<int, int, int> gridPoint;
private:
    void __init(const int resolution[3], int depth, Vec3d bmin, Vec3d bmax);
    void __searchNeighborVoxel(voxel & vcurrentVoxel, std::set<voxel> & vCheckedVoxels, std::vector<voxel>& vScheduledVoxels, int x, int y, int z);
    void __calculateGroupVoxelsIndex();

    inline bool CompareVoxel(voxel& first, voxel& second) { return ((first.first == second.first) && (first.second == second.second) && (first.third == second.third)); }

private:

    //x,y,z轴切分为多少个体素
    int m_Resolution[3];
    int m_Depth;
    const ObjMesh * m_ObjMesh = nullptr;

    std::set<voxel> m_Voxels;

    std::vector<std::set<voxel>> m_GroupVoxels;

    std::vector<std::vector<int>> m_GroupVoxelsIndex;

    //BoundingBox大小，BoundingBox长度，每个小的box的长度 

    Vec3d m_BoundBoxMin, m_BoundBoxMax, m_Side, m_inc;

    SceneObjectDeformable * m_SceneObjectVoxel;

    std::vector<std::vector<double>> m_FileWindConfig;

    int m_Theta;
    int m_Phi;

    std::vector<std::pair<int, int>> m_LineConfig;

    glm::vec3 m_WindDirectionLinePoint;

    Common::SXYZLine m_WindDirectionLine;

    //对应相位pi/2

    double m_FarthestDistance;

    //波动函数的采样cos值按照最初的2pi来进行采样，2pi的cos(x)中采样了60帧\

    double m_Time;

    //默认采用正弦波x为2pi一个周期时候采样60帧，如果频率改变还是2pi的长度采样60帧

    double m_TimeStep;

};



