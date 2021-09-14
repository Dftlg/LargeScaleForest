#include <iostream>
#include <GL/glew.h>
#include "VegaFemFactory.h"

void writePointPosition2File(const std::string& vInputIndex,const std::string& vOutputIndex,const int& vPointIndex, CVegaFemFactory& vFem)
{
	std::ifstream inputFile(vInputIndex);
	if (!inputFile.is_open())
	{
		std::cout << "Can't open the input file!!!" << std::endl;
	}
	std::ofstream outFile;
	outFile.open(vOutputIndex, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the output file!!!" << std::endl;
		return;
	}
	std::string lineString;
	int fileIndex, frameIndex;
	float x = 0, y = 0, z = 0;
	while (getline(inputFile, lineString))
	{
		std::istringstream lineData(lineString);
		lineData >> fileIndex >> frameIndex;
		x = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].x;
		y = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].y;
		z = vFem.getFileFrames(fileIndex)->Frames[frameIndex].BaseFileDeformations[vPointIndex].z;
		outFile <<x <<" " << y<<" " << z << std::endl;
	}
}

void computerMseError(const std::string& interpolationPath, const std::string& groundTruth,std::vector<glm::vec3>& framesError)
{
	std::ifstream inputInterpolationFile(interpolationPath);
	std::ifstream inputGroundTruthFile(groundTruth);
	if (!inputInterpolationFile.is_open()&&!inputGroundTruthFile.is_open())
	{
		std::cout << "Can't open the input file!!!" << std::endl;
	}
	std::string interpolationLineString;
	std::string groundTruthLineString;
	std::string interpolationStr;
	std::string groundTruthStr;
	std::string s;
	glm::vec3 position1;
	glm::vec3 position2;
	

	int i = 0;
	while (getline(inputInterpolationFile, interpolationLineString)&& getline(inputGroundTruthFile, groundTruthLineString))
	{
		s = "Position" + std::to_string(i);
		std::istringstream interpolationLineData(interpolationLineString);
		std::istringstream groundTruthLineData(groundTruthLineString);
		glm::vec3 frameError(0.0);
		interpolationLineData >> interpolationStr;
		if (s == interpolationStr)
		{
			getline(inputInterpolationFile, interpolationLineString);
			getline(inputGroundTruthFile, groundTruthLineString);
			int VertexSize = atoi(interpolationLineString.c_str());
			for (int i = 0; i < VertexSize; i++)
			{
				getline(inputInterpolationFile, interpolationLineString);
				getline(inputGroundTruthFile, groundTruthLineString);
				std::istringstream interpolationLineData1(interpolationLineString);
				std::istringstream groundTruthLineData1(groundTruthLineString);
				interpolationLineData1 >> position1.x >> position1.y >> position1.z;
				groundTruthLineData1 >> position2.x >> position2.y >> position2.z;
				frameError.x += std::abs((position1.x - position2.x)/ position2.x);
				frameError.y += std::abs((position1.y - position2.y)/ position2.y);
				frameError.z += std::abs((position1.z - position2.z)/position2.z);
			}
			/*frameError.x = frameError.x / VertexSize;
			frameError.y = frameError.y / VertexSize;
			frameError.z = frameError.z / VertexSize;*/
			framesError.push_back(frameError);
			i++;
		}
	}

	glm::vec3 sum(0.0);
	////deltaU
	/*for (int i = 0; i < framesError.size();i++)
	{
		framesError[i].x = (framesError[i].x / 35132)*100;
		framesError[i].y = (framesError[i].y / 35132)*100;
		framesError[i].z = (framesError[i].z / 35132)*100;
	}*/

	//U
	for (int i = 0; i < framesError.size(); i++)
	{
		framesError[i].x = ((framesError[i].x + sum.x)/ 35132)*100;
		framesError[i].y = ((framesError[i].y + sum.y)/ 35132)*100;
		framesError[i].z = ((framesError[i].z + sum.z)/ 35132)*100;
		sum.x += framesError[i].x;
		sum.y += framesError[i].y;
		sum.z += framesError[i].z;
		
	}
}

void write2File(std::string vPath, std::vector<glm::vec3>& framesError)
{
	std::cout << "Writing the result to the file !!!" << std::endl;
	std::ofstream outFile(vPath);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	for (int i = 0; i < framesError.size(); i++)
	{
		outFile << framesError[i].x << " " << framesError[i].y << " " << framesError[i].z << "\n";
	}
	outFile.close();
}


int main()
{
	//CVegaFemFactory vFem("G:/GraduationProject/yellow_tree/deltaU", "../../models/yellow_tree/tree_last.obj", "../../models/yellow_tree/ObjectVertexIndex.txt");
	//int numbercounter = 6;
	//std::vector<double> b{ 1000,1,0,0 };
	//std::vector<std::pair<int, int>> angle;
	//std::vector<Common::SFileFrames> vtemp = vFem.searchFileFrameOnAttribute();
	//for (int i = 0; i < numbercounter; i++)
	//{
	//	std::vector<Common::SFileFrames> temp;
	//	temp.push_back(vtemp[i]);
	//	vFem.readFramesDeformationData(temp, i);
	//}
	//writePointPosition2File("G:/GraduationProject/yellow_tree/xyz/FileAndFrameIndex.txt", "G:/GraduationProject/yellow_tree/xyz/Interpolation_1150_33971_deltaU.txt", 33971, vFem);

	std::vector<glm::vec3> framesError;
	computerMseError("D:/GraduationProject/LargeScaleForest/models/yellow_tree/displaceInterpo21.txt", "D:/GraduationProject/LargeScaleForest/models/yellow_tree/displaceGroundTruth.txt", framesError);
	write2File("G:/pythonProject2/third/UmseError.txt", framesError);
	return 0;
}