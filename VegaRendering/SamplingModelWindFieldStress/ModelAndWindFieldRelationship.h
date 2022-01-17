#pragma once
#include "pch.h"
#include <fstream>
#include <iostream>
#include<set>
#include"triangle.h"
#include "objMesh.h"
#include "triple.h"
#include "boundingBox.h"

//主要计算定向风源风源模型参看i3d 2017 Responsive Real-Time  Grass Rendering for General 3D Scenes

class CModelAndWindFieldRelationShip
{
public:
    CModelAndWindFieldRelationShip() = default;
    CModelAndWindFieldRelationShip(const ObjMesh * objMesh, const int resolution[3], int depth = 0, double expansionFactor = 1.2);
    // boxbmin, boxbmax, explicitly provide the bounding box
    CModelAndWindFieldRelationShip(const ObjMesh * objMesh, const int resolution[3], int depth, Vec3d boxbmin, Vec3d boxbmax);

    void SaveCorrespondingVoxelOfTreeGroup(const std::string & vDirectionPath);

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

};



