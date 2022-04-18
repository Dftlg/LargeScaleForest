#include "pch.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Hausdorff.h"

using namespace std;

void readData(string vPath, vector<vector<double>>&voData)
{
    std::ifstream readFile(vPath);
    if (!readFile)
    {
        std::cout << "Cant't open the file!!!" << std::endl;
    }

    string lineString;
    while (getline(readFile, lineString))
    {
        std::istringstream tempString(lineString);
        vector<double> singleVertex;
        glm::vec3 tempVertex;
        tempString >> tempVertex.x >> tempVertex.y >> tempVertex.z;
        singleVertex.push_back(tempVertex.x);
        singleVertex.push_back(tempVertex.y);
        singleVertex.push_back(tempVertex.z);
        voData.push_back(singleVertex);
    }
    readFile.close();
}

void getDataSum(string vDirPath, int vFirstNum, vector<vector<double>>&voDSumData)
{
    vector<vector<double>> data;
    for (int i = 0; i < 5; i++)
    {
        string path = vDirPath + to_string(i + vFirstNum) + "Tru.txt";
        readData(path, data);
        for (int k = 0; k < data.size(); k++)
        {
            for (int j = 0; j < 3; j++)
            {
                voDSumData[k][j] += data[k][j];
            }
        }
        vector<vector<double>>().swap(data);
    }
}

void writeData(string vPath, vector<vector<double>> vSumData)
{
    std::ofstream  outFile(vPath, std::ios::in | std::ios::app);
    for (int i = 0; i < vSumData.size(); i++)
    {
        for (int k = 0; k < 3; k++)
        {
            outFile << vSumData[i][k] << " ";
        }
        outFile << "\n";
    }
    outFile.close();
}

float CalculateAverageError(const std::string& vTruthPath, const std::string& vInpoPath, int vReadFirstIndex, int vReadLastIndex)
{
    double p = 1;//放大误差范围
    Average_Hausdorff A_H_Obj;
    string filenamePareto, filenameSet;
    double SumError=0;
    for (int i = vReadFirstIndex; i < vReadLastIndex; i++)
    {
        filenamePareto = vTruthPath + to_string(i) + ".txt";
        filenameSet = vInpoPath + to_string(i) + ".txt";
        float tempHausdorffError= A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p);
        std::cout << tempHausdorffError << std::endl;
        SumError += tempHausdorffError;
    }
    SumError /= (vReadLastIndex - vReadFirstIndex);
    return SumError;
}

int main(int argc, char *argv[])
{
    double p = 1;//放大误差范围
    bool Normalize = false;
    string fileTruthPathRoot = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment4_3_1/voxelnumber35/groundtruth600frameobj/";
    string fileInpoPathRoot = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment4_3_1/voxelnumber35/motionSearch600frameobj/";
    //string fileTruthPathRoot = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment4_3_1/voxelnumber35/groundtruthObj/";
    //string fileInpoPathRoot = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/mini_mapleTree/FixedKOthrVegType/70/experiment4_3_1/voxelnumber35/motionSearchObj/";
    string filenamePareto, filenameSet;
    Average_Hausdorff A_H_Obj;
    A_H_Obj.setNormalize(Normalize);


    /*filenamePareto = fileTruthPathRoot  + "40.txt";
    filenameSet = fileInpoPathRoot  + "40.txt";
    std::cout << 40 << "error" << A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p) << endl;

    filenamePareto = fileTruthPathRoot + "100.txt";
    filenameSet = fileInpoPathRoot + "100.txt";
    std::cout << 100 << "error" << A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p) << endl;

    filenamePareto = fileTruthPathRoot + "160.txt";
    filenameSet = fileInpoPathRoot + "160.txt";
    std::cout << 160 << "error" << A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p) << endl;*/

    float tempdata= CalculateAverageError(fileTruthPathRoot, fileInpoPathRoot, 0, 600);
    std::cout<< tempdata;
    std::ofstream  outFile("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/FixedKOthrVegType/70/experiment4_3_1/voxelnumber35/600帧平均误差.txt", std::ios::in | std::ios::app);
    outFile << tempdata;
    outFile.close();

    /* cout << endl << "误差为：";
     filenamePareto = "J:/Experiment/apricot_tree/SimilarityError/SimiarityDeltaU_5/24.txt", filenameSet = "J:/Experiment/apricot_tree/SimilarityError/SimiarityDeltaU_5/4.txt";
     cout << A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p) << endl;*/



     /* cout << endl << "14和19的误差：";
      filenamePareto = "14.txt", filenameSet = "19.txt";
      cout << A_H_Obj.do_Metric_Hausdorff(filenamePareto, filenameSet, p) << endl;*/
    return 0;
}
