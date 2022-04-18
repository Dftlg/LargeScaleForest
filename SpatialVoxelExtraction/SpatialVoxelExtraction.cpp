// SpatialVoxelExtraction.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//calculate Labeled voxel

//用于将veg文件中不同的set进行进一步依靠刚度值和模型质量拓扑结构提取重要骨架，参考论文3.4章

#include "pch.h"
#include <iostream>
#include "../VegaRendering/Common/common.h"
#include "../VegaRendering/RenderingProcess/CubicVeg.h"
#include <limits>
#pragma optimize("", off)

int main()
{
   
    //CubicVegMesh* VegMesh = nullptr;
    //VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/tree70.veg","",true,false);
    ////VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/mapleTree70.veg", "", true, false);
    ////VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/apricot_tree/FixedKOthrVegType/70/apricotree.veg", "", true, false);

    //VegMesh->SetMassAndMaterialCalulacteValueRelated(2, 1);
    //VegMesh->ConstructVoxelGroup();

    ////每个组的体素个数

    //std::vector<int> tempBaseGroupVoxel = VegMesh->GetAfterEraseRegionVoxelNumber();
    ////yellowtree
    ////70*70*70的体素三个group部分为310，658，733
    //std::vector<int> GroupEraseNumber = { 163,511,586 };
    ////std::vector<int> GroupEraseNumber = { 307,655,729 };
    ////std::vector<int> GroupEraseNumber = { 265,613,688 };
   
    ////mapletree
    ////70*70*70的体素两个group部分为103，1105
    ////std::vector<int> GroupEraseNumber = { 93,377 };
    ////std::vector<int> GroupEraseNumber = { 36,1037 };

    ////70*70*70的体素两个group部分为315，1361
    ////std::vector<int> GroupEraseNumber = { 310,1331 };
    ////std::vector<int> GroupEraseNumber = { 247,1293 };


    //for (int GroupIndex = 0; GroupIndex < GroupEraseNumber.size(); GroupIndex++)
    //{
    //    for (int i = 0; i < GroupEraseNumber[GroupIndex]; i++)
    //    {
    //        VegMesh->EraseMaxValueVoxelWithAllChildGroup(GroupIndex);
    //    }
    //}


    ////前面一半数是没进行剔除的每个group体素个数，后面一半是剔除后的个数

    //std::vector<int> tempGroupVoxel= VegMesh->GetAfterEraseRegionVoxelNumber();

    //yellowtree
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber10/EachPartSkelCubic_index");
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/FixedKOthrVegType/70/experiment1/voxelnumber135/EachPartSkelCubic_index");

    //mapletree
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment1/voxelnumber35/EachPartSkelCubic_index");
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment1/voxelnumber135/EachPartSkelCubic_index");

    //apricotree
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/apricot_tree/FixedKOthrVegType/70/experiment1/voxelnumber35/EachPartSkelCubic_index");
    //VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/apricot_tree/FixedKOthrVegType/70/experiment1/voxelnumber135/EachPartSkelCubic_index");
    //
    ///////////////////////////////////////////////////////
    //前半部分为剔除依靠结构信息剔除部分提速，后半部分为剔除随机体素

    CubicVegMesh* VegMeshsec = nullptr;
    //VegMeshsec = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/tree70.veg", "", true, false);
    VegMeshsec = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/tree70_exp4.veg", "", true, false);
    
    VegMeshsec->SetMassAndMaterialCalulacteValueRelated(2, 1);
    VegMeshsec->ConstructVoxelGroup();
    int AllErase = 0;
    //for (auto nu : GroupEraseNumber)
    //    AllErase += nu;
    std::vector<int> tempVoxelNumber = VegMeshsec->GetAfterEraseRegionVoxelNumber();
    //10037
    //AllErase = 10002;
    //AllErase = 9966;
    //int sumnumber = 0;
    //for (auto i : tempVoxelNumber)
    //    sumnumber += i;
    //for (int i = 0; i < AllErase; i++)
    //{
    //    VegMeshsec->EraseRandomVoxel();
    //    //std::vector<int> test = VegMeshsec->GetAfterEraseRegionVoxelNumber();
    //}

    //std::vector<int> GroupEraseNumber = { 8291,305,650,720 };
    std::vector<int> GroupEraseNumber = { 8289,298,645,695 };
    for (int i = 0; i < GroupEraseNumber.size(); i++)
    {
        for (int k = 0; k < GroupEraseNumber[i]; k++)
        {
            VegMeshsec->EraseSpecGroupRandomVoxel(i);
        }
    }

    VegMeshsec->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber110/ReSetGraphRandomSelectEachPartSkelCubic_index");
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
#pragma optimize("", on)
