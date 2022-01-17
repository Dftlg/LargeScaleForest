// VegFileDivideMaterialsRegions.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//正常情况下模型的刚度值设置困难，因为无法知道模型在veg中的准确位置，不好分割设置刚度值，
//该项目用于将obj文件不同的part映射到veg中进行模型分割生成在veg文件中的代表的Regions的体素索引，以此设置不同的刚度值

#include "pch.h"
#include <iostream>
#include "objMesh.h"
#include"ObjMappingVoxel.h"

int main()
{
    //将模型x,y,z轴分为100个体素
    int VoxelInputData = 70;
    ObjMesh * newObjMesh;
    newObjMesh = new ObjMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/li_final_tree_last.obj");
    double expansionFactor = 1.2;
    int depth = 0;
    int ResolutionArray[3] = { VoxelInputData, VoxelInputData, VoxelInputData };
    CObjMappingVoxel * objMeshVoxels = new CObjMappingVoxel(newObjMesh, ResolutionArray, depth, expansionFactor);
    objMeshVoxels->CalculateEachGroupsVoxels();
    objMeshVoxels->RemoveOverlappingVoxelsInTwoGroups(1, 3);
    objMeshVoxels->RemoveOverlappingVoxelsInTwoGroups(2, 0);
    //objMeshVoxels->RemoveOverlappingVoxelsInTwoGroups(0, 1);
    objMeshVoxels->SaveCorrespondingVoxelOfTreeGroup("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/ObjMappingVoxel");

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
