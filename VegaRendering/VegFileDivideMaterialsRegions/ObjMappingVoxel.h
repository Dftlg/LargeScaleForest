#pragma once
#include "pch.h"
#include <fstream>
#include <iostream>
#include<set>
#include"triangle.h"
#include "objMesh.h"
#include "triple.h"
#include "boundingBox.h"

class CObjMappingVoxel
{
public:
    CObjMappingVoxel() = default;
    CObjMappingVoxel(const ObjMesh * objMesh, const int resolution[3], int depth = 0, double expansionFactor = 1.2);
    // boxbmin, boxbmax, explicitly provide the bounding box
    CObjMappingVoxel(const ObjMesh * objMesh, const int resolution[3], int depth, Vec3d boxbmin, Vec3d boxbmax);

    void CalculateEachGroupsVoxels();

    void SaveCorrespondingVoxelOfTreeGroup(const std::string & vDirectionPath);

    //为了使得体素更加清晰的表明，将两个组内的重叠部分体素删除，将重叠部分的体素保留在vReserveGroupsIndex组内，二vRemoveGroupsIndex组体素删除
    //参考D:\GraduationProject\New-LargeScaleForest\LargeScaleForest\models\yellow_tree\OtherVegType\100 的Readme

    void RemoveOverlappingVoxelsInTwoGroups(int vRemoveGroupsIndex, int vReserveGroupsIndex);

    typedef triple<int, int, int> voxel;
    typedef triple<int, int, int> gridPoint;
private:
    void __init(const int resolution[3], int depth, Vec3d bmin, Vec3d bmax);
    void __searchNeighborVoxel(voxel & vcurrentVoxel, std::set<voxel> & vCheckedVoxels, std::vector<voxel>& vScheduledVoxels, int x, int y, int z);
    void __calculateGroupVoxelsIndex();

    inline bool CompareVoxel(voxel& first,voxel& second){ return ((first.first == second.first) && (first.second == second.second) && (first.third == second.third)); }

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


