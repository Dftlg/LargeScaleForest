// SpatialVoxelExtraction.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//calculate Labeled voxel

#include "pch.h"
#include <iostream>
#include "../VegaRendering/Common/common.h"
#include "../VegaRendering/RenderingProcess/CubicVeg.h"
#include <limits>
#pragma optimize("", off)

int main()
{
   
    CubicVegMesh* VegMesh = nullptr;
    VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/tree100.veg","",true,false);
    //VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/tree100.veg", true);
    VegMesh->SetMassAndMaterialCalulacteValueRelated(2, 1);
    VegMesh->ConstructVoxelGroup();

    std::vector<int> tempBaseGroupVoxel = VegMesh->GetAfterEraseRegionVoxelNumber();
    for (int i = 0; i < 120; i++)
    {
        VegMesh->EraseMaxValueVoxelWithAllChildGroup(0);
    }
    std::cout << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 155; i++)
    {
        if (i == 85)
        {
            int k = 0;
        }
        VegMesh->EraseMaxValueVoxelWithAllChildGroup(1);
        
    }

    std::cout << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << std::endl;
    for (int i = 0; i <470 ; i++)
    {
        VegMesh->EraseMaxValueVoxelWithAllChildGroup(2);
    }

    std::vector<int> tempGroupVoxel= VegMesh->GetAfterEraseRegionVoxelNumber();

    VegMesh->SaveKeyStiffnessVoxel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/EachPartSkelCubic_index");
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
