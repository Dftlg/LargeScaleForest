#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <omp.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>


void readDataFromVegFile(std::string vPath, std::vector<glm::vec3>&voElementVertices, std::vector<std::vector<int>>& voElementIndex)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Veg file can't open the file!!!" << std::endl;
		return;
	}
	std::cout << "Read element's index and element's vertices index from veg file!!!" << std::endl;
	bool flag=true, verticesFlag = false, cubicIndexFlag = false;
	std::string lineString;
	while (flag)
	{
		getline(inFile, lineString);
		if (lineString == "*VERTICES")
		{
			verticesFlag = true;
			getline(inFile, lineString);
		}
		while (verticesFlag)
		{
			getline(inFile, lineString);
			if (lineString == "*ELEMENTS")
			{
				verticesFlag = false;
				cubicIndexFlag = true;
				getline(inFile, lineString);
				getline(inFile, lineString);
				break;
			}	
			if (lineString != "")
			{
				std::istringstream tempString(lineString);
				glm::vec3 tempVertex;
				float temp;
				tempString >> temp >> tempVertex.x >> tempVertex.y >> tempVertex.z;
				voElementVertices.push_back(tempVertex);
			}
		}
		while (cubicIndexFlag)
		{
			getline(inFile, lineString);
			if (lineString != "")
			{
				std::istringstream tempString(lineString);
				std::vector<int>tempCubicIndex;
				int tempSingleIndex;
				tempString >> tempSingleIndex;
				for (int i = 0; i < 8; i++)
				{
					tempString >> tempSingleIndex;
					tempCubicIndex.push_back(tempSingleIndex);
				}
				voElementIndex.push_back(tempCubicIndex);
			}
			else
			{
				cubicIndexFlag = false;
				flag = false;
			}
		}
	}
	inFile.close();
}

void readDataFromObjFile(std::string vPath, std::vector<glm::vec3>&voObjVertices)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Obj file can't open the file!!!" << std::endl;
		return;
	}
	std::cout << "Read model's vertices from obj file!!!" << std::endl;
	std::string lineString;
	while (getline(inFile, lineString))
	{
		std::istringstream tempString(lineString);
		std::string tempChar;
		tempString >> tempChar;
		if (tempChar == "v")
		{
			glm::vec3 tempVertex;
			tempString >> tempVertex.x >> tempVertex.y >> tempVertex.z;
			voObjVertices.push_back(tempVertex);
		}
		if (tempChar == "vt" || tempChar == "vn")
		{
			continue;
		}
	}
	inFile.close();
}

void readDataFromSkeleronFile(std::string vPath, std::vector<glm::vec3>&voObjVertices)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Skelton file can't open the file!!!" << std::endl;
		return;
	}

	std::cout << "Read skelton's vertices from skel file!!!" << std::endl;
	std::string lineString;
	bool flag = true;
	while (getline(inFile, lineString)&& flag)
	{
		std::istringstream tempString(lineString);
		std::string tempChar;
		tempString >> tempChar;
		if (tempChar == "SN")
		{
			while (getline(inFile, lineString)&& lineString != "")
			{
				std::istringstream tempString(lineString);
				std::string tempChar; 
				glm::vec3 tempVertex;
				//tempString >> tempChar >> tempChar >> tempChar;
				tempString >> tempVertex.x >> tempVertex.y >> tempVertex.z;
				voObjVertices.push_back(tempVertex);
			}
			flag = false;
		}
	}
	inFile.close();
}

void readDataFromTxt(std::string vPath, std::vector<int>&voTxtIndex)
{
	std::ifstream inFile(vPath);
	if (!inFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}

	std::cout << "Read element's index from txt file!!!" << std::endl;
	std::string lineString;
	while (getline(inFile, lineString))
	{
		std::istringstream tempString(lineString);
		std::vector<std::string> lineStringSplit;
		boost::split(lineStringSplit, tempString.str(), boost::is_any_of(","), boost::token_compress_off);
		for (int i = 0; i < lineStringSplit.size()-1; i++)
		{
			voTxtIndex.push_back(atoi(lineStringSplit[i].c_str()));
		}
		
	}

}

//查找顶点的cubic索引号
void findCubicIndexForVertices(std::vector<glm::vec3>& vObjVertices, std::vector<glm::vec3>&vElementVertices, std::vector<std::vector<int>>& vElementIndex, std::vector<int>& vElementIndexForObjVertices)
{
	std::cout << "Begin find element's index for the vertice!!!" << std::endl;
	float startTime = omp_get_wtime();
#pragma omp parallel for //num_threads(4)
	for (int i = 0; i < vObjVertices.size(); i++)
	{
		for (int k = 0; k < vElementIndex.size(); k++)
		{
			//isNoneVertex.push_back(0);
			std::vector<float>tempX, tempY, tempZ;
			for (int j = 0; j < vElementIndex[0].size(); j++)
			{
				tempX.push_back(vElementVertices[vElementIndex[k][j] - 1].x);
				tempY.push_back(vElementVertices[vElementIndex[k][j] - 1].y);
				tempZ.push_back(vElementVertices[vElementIndex[k][j] - 1].z);
			}
			float minX = *min_element(tempX.begin(), tempX.end());
			float maxX = *max_element(tempX.begin(), tempX.end());
			float minY = *min_element(tempY.begin(), tempY.end());
			float maxY = *max_element(tempY.begin(), tempY.end());
			float minZ = *min_element(tempZ.begin(), tempZ.end());
			float maxZ = *max_element(tempZ.begin(), tempZ.end());
			if (vObjVertices[i].x>minX && vObjVertices[i].x < maxX && vObjVertices[i].y> minY && vObjVertices[i].y < maxY && vObjVertices[i].z > minZ && vObjVertices[i].z < maxZ)
			{
				vElementIndexForObjVertices.push_back(k + 1);
				k = vElementIndex.size();
				//isNoneVertex.pop_back();
			}
		}
	}
	float endTime = omp_get_wtime();
	float time = endTime - startTime;
	//std::cout << "指定 5 个线程，执行时间: " << time << std::endl;
	sort(vElementIndexForObjVertices.begin(), vElementIndexForObjVertices.end());
	vElementIndexForObjVertices.erase(std::unique(vElementIndexForObjVertices.begin(), vElementIndexForObjVertices.end()), vElementIndexForObjVertices.end());
}

//通过cubic的顶点索引号查找对应的cubic索引号 
void findCubicIndexByVerticeIndex(std::vector<int>vCubicVerticeIndex, std::vector<std::vector<int>>& vElementIndex,std::vector<int>&vElementIndexForObjVertices)
{
	for (int i = 0; i < vCubicVerticeIndex.size(); i++)
	{
		for (int k = 0; k < vElementIndex.size(); k++)
		{
			for (int j = 0; j < vElementIndex[0].size(); j++)
			{
				if (vElementIndex[k][j] == vCubicVerticeIndex[i])
					vElementIndexForObjVertices.push_back(k + 1); 
					break;
			}
		}
	}
}

//查找给定的cubic索引号对应的八个顶点的索引值
void findCubicVertices(std::vector<std::vector<int>> vElementIndex, std::vector<int>& vElementIndexForObjVertices, std::vector<int>& voFindElementVertices)
{
	std::cout << "Begin find element's vertices for the element!!!" << std::endl;
	for (int i = 0; i < vElementIndexForObjVertices.size(); i++)
	{
		for (int k = 0; k < vElementIndex[0].size(); k++)
		{
			voFindElementVertices.push_back(vElementIndex[vElementIndexForObjVertices[i] - 1][k]);
		}
	}
}

void findMissedCubicIndex(float vertexPos, std::vector<glm::vec3>&vElementVertices, std::vector<std::vector<int>>& vElementIndex, std::vector<int>& vElementIndexForObjVertices)
{
	std::cout << "Begin find missed element's index !!!" << std::endl;
//#pragma omp parallel
	for (int i = 0; i < vElementIndex.size(); i++)
	{
		for (int k = 0; k < vElementIndex[0].size(); k++)
		{
			if (vElementVertices[vElementIndex[i][k] - 1].y < vertexPos)
			{
				vElementIndexForObjVertices.push_back(i + 1);
				break;
			}
		}
	}
}

void removeDuplicatd(std::vector<int>& vOperateObject, std::vector<int>&vConstrastObject , std::vector<int>&voTargetIndex)
{
	for (int i = 0; i < vOperateObject.size(); i++)
	{
		if (std::count(vConstrastObject.begin(), vConstrastObject.end(), vOperateObject[i]) ==0)
		{
			voTargetIndex.push_back(vOperateObject[i]);
		}
	}
}

void write2File(std::string vPath, std::vector<int>& vElementIndexForObjVertices)
{
	std::cout << "Writing the result to the file !!!" << std::endl;
	std::ofstream outFile(vPath);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	for (int i = 0; i <vElementIndexForObjVertices.size(); i++)
	{
		outFile << vElementIndexForObjVertices[i] << ",";
	}
	outFile.close();
}




int main()
{
	std::vector<glm::vec3> elementVertices;
	std::vector<glm::vec3>objVertices;
	std::vector<std::vector<int>> elementIndex;
	std::vector<int> elementIndexForObjVertices;
	std::vector<int> elementIndexForObjVertices1;
	std::vector<int> elementIndexForObjVertices2;
	std::vector<int> findedElementVertices;
	std::string vegFilePath = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/tree100.veg";
	std::string objFilePath = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree_last_test.obj";
	std::string skeletonFilePath = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/treeply/treeskel/stem_test.skel";
	std::string cubicIndexFilePath1 = "D:/GraduationProject/LargeScaleForest/models/yellow_tree_skeleton/test.bou";
	std::string cubicIndexFilePath2 = "D:/GraduationProject/LargeScaleForest/models/yellow_tree_skeleton/stem_003_cubic_index.txt";
	//std::string cubicIndexFilePath3 = "D:/GraduationProject/Vega/models/10.20/fibrous_all_cubic_index1.txt";
	std::string cubicIndexOutputPath = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/EachPartSkelCubic_index/stem_test_cublic_index.txt";
	std::string cubicVerticesOputputPath = "D:/GraduationProject/LargeScaleForest/models/yellow_tree_skeleton/test_vertices_index.bou";

#pragma region findCubicIndex
	readDataFromVegFile(vegFilePath, elementVertices, elementIndex);
	readDataFromSkeleronFile(skeletonFilePath, objVertices);
	//readDataFromObjFile(objFilePath, objVertices);
	findCubicIndexForVertices(objVertices, elementVertices, elementIndex, elementIndexForObjVertices);
	//findMissedCubicIndex(1.8115, elementVertices, elementIndex, elementIndexForObjVertices);
#pragma endregion

#pragma region removeDuplicatedIndex
	//readDataFromTxt(cubicIndexFilePath1, elementIndexForObjVertices1);
	//readDataFromTxt(cubicIndexFilePath2, elementIndexForObjVertices2);
	//removeDuplicatd(elementIndexForObjVertices, elementIndexForObjVertices2, elementIndexForObjVertices3);
#pragma endregion

#pragma region findCubicIndexByVerticeIndex
	//findCubicIndexByVerticeIndex(elementIndexForObjVertices1, elementIndex, elementIndexForObjVertices);
#pragma endregion

	
#pragma region findCubicVertices
	//readDataFromTxt(cubicIndexFilePath1, elementIndexForObjVertices);
	//findCubicVertices(elementIndex, elementIndexForObjVertices, findedElementVertices);
#pragma endregion

#pragma region write2File
	write2File(cubicIndexOutputPath, elementIndexForObjVertices);
	//write2File(cubicVerticesOputputPath, findedElementVertices);
#pragma 

	system("pause");
}
