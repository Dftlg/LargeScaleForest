// AuxiliaryProjectFeatures.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include<sstream>
#include <io.h>

void readFileDirectory(const std::string & vDirectoryName);
void changeDirectoryFileName(const std::string & vDirectoryName);
void AddTestEachFileForceFile(const std::string & vDirectoryName);

std::vector<std::string> m_FilePathList;
std::vector<std::string> m_FileSpkvf;

//修改某文件夹下文件名称功能主要为了实验更改项目数据

void readFileDirectory(const std::string & vDirectoryName)
{
    intptr_t  hFile = 0;
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(vDirectoryName).append("\\*.txt").c_str(), &fileinfo)) != -1)
    {
        do
        {
            m_FilePathList.push_back(p.assign(vDirectoryName).append("/").append(fileinfo.name));

        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }

    intptr_t  hFile1 = 0;
    struct _finddata_t fileinfo1;
    std::string p1;
    if ((hFile1 = _findfirst(p1.assign(vDirectoryName).append("\\*.spkvf").c_str(), &fileinfo1)) != -1)
    {
        do
        {
            m_FileSpkvf.push_back(p1.assign(vDirectoryName).append("/").append(fileinfo1.name));

        } while (_findnext(hFile1, &fileinfo1) == 0);
        _findclose(hFile1);
    }
}

void changeDirectoryFileName(const std::string & vDirectoryName)
{
    readFileDirectory(vDirectoryName);
    int index = 13;
    for (int i = 0; i < m_FilePathList.size(); i++)
    {
        std::string tempstring = "";
        //std::string tem = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/18/motiondata\\positionthe0phi60force1Index";
        tempstring = tempstring + "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/18/motiondata\\positionthe0phi60force1Index";
        tempstring = tempstring + std::to_string(i + 13);
        tempstring = tempstring + ".txt";
        if (!rename(m_FilePathList[i].c_str(), tempstring.c_str()))
        {
            std::cout << "rename success " << std::endl;
        }
        else
        {
            std::cout << "rename error " << std::endl;
        }


    }

    for (int i = 0; i < m_FileSpkvf.size(); i++)
    {
        std::string tempstring = "";
        //std::string tem = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/18/motiondata\\positionthe0phi60force1Index";
        tempstring = tempstring + "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/18/motiondata\\positionthe0phi60force1Index";
        tempstring = tempstring + std::to_string(i + 13);
        tempstring = tempstring + ".spkvf";
        rename(m_FileSpkvf[i].c_str(), tempstring.c_str());
    }
}

int main()
{
    //changeDirectoryFileName("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/18/motiondata");

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
