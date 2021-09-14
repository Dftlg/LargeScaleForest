#pragma once



#include <iostream>
#include <fstream>
#include<sstream>
#include <assert.h>
#include <thread>
#include <time.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "common.h"
#include "ExtraTool.h"
#include "ModelDeformationTransform.h"
#include <omp.h>
#include "../Common/WindFiled.h"


class CVegaFemFactory
{
public:

	CVegaFemFactory() = default;
	CVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile, const std::string &vCorrectDeformationUVertexIndex);
	~CVegaFemFactory() = default;

	void readCorrectUdeformationIndex(const std::string & vFilePath);
	void readKVFFileData(const std::string & vFile, Common::SFileFrames & vFileFrame);
	void readUdeformationData(const std::string & vFile, Common::SFileFrames &vFileFrame);
	void readFilePath4Directory(const std::string & vDirectoryName);
	void write2File(const std::string& vPath, std::vector <std::pair<int, double>>& vOutputData, std::vector <std::pair<int, double>>& vForceFrame, std::vector <std::pair<int, double>>& vKVfFrame, int vFrameIndex);
	void writeFindFrameIndex2File(const std::string& vPath,std::pair<int, int>& vTempTreeFileAndFrameIndex);
	void readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex);
	void readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData);
	void readDeformationDataByMutileThread(Common::SFileFrames& vBaseFileFramesStruct, const std::string& vFilePath, int vSFileFramesIndex);
	void readDeformationDataInOneThread(Common::SFileData& vFileData, int vTid, const std::string& vFilePath, long long vBlockSize);
	void addSeekgOfEachFramesBlock(const std::string& vFilePath);
	std::vector<Common::SFileDataGroup> getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep);
	//根据文件的绝对路径取文件名字,如：positionthe0phi0force500,500,500
	std::string getFileName(const std::string & vFileDirectory);
	//返回一个形变文件
	//Common::SFileFrames getFileFrames(int vIndex) { return m_FilesData[vIndex]; }
	Common::SpKVFData getFirstKVFDataFromFirstFileFrame() { return m_FilesData[0].KVFFrameDatas[0]; }

	Common::SFileFrames* getFileFrames(int vIndex) { return &m_FilesData[vIndex]; }
	std::vector<std::vector<int>> getMultipleFramesIndex() { return m_MultipleFramesIndex; }

	std::pair<int, int> getFileAndFrameIndex(int vTreeIndex, int vFrameIndex) { return m_MultipleFileAndFramesIndex[vTreeIndex][vFrameIndex]; };
	int getSearchFileIndex(int vTreeIndex, int vFrameIndex) { return m_MultipleFileAndFramesIndex[vTreeIndex][vFrameIndex].first; };
	int getSearchFrameIndex(int vTreeIndex, int vFrameIndex) { return m_MultipleFileAndFramesIndex[vTreeIndex][vFrameIndex].second; };

	void getDisplacementTrajectory(std::vector<std::vector<glm::vec3>>& PositionsDeformation);
	std::vector<Common::SConnectedFemFiles> getAllReallyLoadConnectedFem() { return m_AllReallyLoadConnectedFem; }

	void setDeformationStateFromFileName();
	CModelDeformationTransform*  getModelTransformStruct() { return m_ModelTransformStruct;}
	std::vector<Common::SFileFrames> searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<double> & vForceFluctuationSequence);
	std::vector<Common::SFileFrames> searchFileFrameOnAttribute();

	double getKMatrixSumNumber()
	{
		double Sum = 0;
		if (!m_AllReallyLoadConnectedFem.empty())
			for(int i=0;i< m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths.size();i++)
				Sum+= m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths[i];
		return Sum;
	}
	std::vector<int> getKMatrixEachElementRelatedDataLength() { if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].KLengths; }
	double getVeocitySumNumber() {if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].Velocity.size();}
	double getInternalForceSumNuber() { if (!m_AllReallyLoadConnectedFem.empty()) return m_AllReallyLoadConnectedFem[0].FemDataset[0]->KVFFrameDatas[0].InternalForces.size(); }

	void cleanSFileDataGroup(int vConnectionIndex, int vTimestep);

	void initMatchedFrameStruct(int vTreeSize);

	void initTempMultipleTreeData(int vTreeSize);

	void initKVFDataSearchRangeError();

	void resetTempMultipleTreeData(int vTreeSize);
	
	void searchMatchedDeformationFrames(std::vector<glm::vec3> & vUdeltaDeformation,std::vector<glm::vec3> & vFrameUDeformationData);

	void searchMatchedFrameSequences(std::vector<std::vector<int>> &voExtraForces, std::vector<std::vector<Common::SForceDirection>> &vForceDirection);

	void searchMatchedOneTreeFrameSequences(std::vector<int> & voMatchedFrameSequenceIndex, Common::SpKVFData& voSpKVData, std::vector<int>&voExtraForces, int &voCurrentFrameIndex, int& vIsFirstFrame);

	inline float addDistance(float vorigin) { return vorigin + 0.0005; };
	inline float minusDistance(float vorigin) { return vorigin - 0.0005; };
	inline int distanceError(glm::vec3 vorigin, glm::vec3 vinput) 
	{ 
		if ((addDistance(vorigin.x) > vinput.x&& minusDistance(vorigin.x) < vinput.x) &&
			(addDistance(vorigin.y) > vinput.y&& minusDistance(vorigin.y) < vinput.y) &&
			addDistance(vorigin.z) > vinput.z&& minusDistance(vorigin.z) < vinput.z)
			return 1;
		return 0;
	};
	inline int SquareError(int vFirstNumber, int vSecondNumber)
	{
		return (vFirstNumber - vSecondNumber)*(vFirstNumber - vSecondNumber);
	};
	inline double SquareError(double vFirstNumber, double vSecondNumber)
	{
		return (vFirstNumber - vSecondNumber)*(vFirstNumber - vSecondNumber);
	};
	inline int AbsError(int vFirstNumber, int vSecondNumber)
	{
		return abs(vFirstNumber - vSecondNumber);
	};
	inline bool AbsError(double vFirstNumber, double vSecondNumber,double vJudgeRange)
	{
		return (abs(vFirstNumber - vSecondNumber) < vJudgeRange);
	};
	inline bool AbsError(glm::vec3 vFirstNumber, glm::vec3 vSecondNumber, double vJudegRange)
	{
		if (abs(vFirstNumber.x - vSecondNumber.x) < vJudegRange && 
			abs(vFirstNumber.y - vSecondNumber.y) < vJudegRange &&
			abs(vFirstNumber.z - vSecondNumber.z) < vJudegRange)
			return true;
		return false;
	};
	inline bool AbsError(glm::vec3 vFirstNumber, glm::vec3 vSecondNumber, double vJudegRange,int vReduce)
	{
		if (abs((vFirstNumber.x - vSecondNumber.x)/vReduce) < vJudegRange &&
			abs((vFirstNumber.y - vSecondNumber.y)/vReduce) < vJudegRange &&
			abs((vFirstNumber.z - vSecondNumber.z)/vReduce) < vJudegRange)
			return true;
		return false;
	};
	inline double GaussianFunction(double vVariable,double vSigma=1, double vMiu=0,double vTruncated=0.001)
	{
		double GaussianValue=(1 / (vSigma*sqrt(2 * Common::Pi))*exp(-0.5*pow((vVariable - vMiu) / vSigma, 2)));
		if (GaussianValue < vTruncated)
			GaussianValue = 0;
		return GaussianValue;
	}


private:

	void __getFileSeekDirOfEachBlock(const std::string& vFilePath, std::vector<long long>& vBlock);
	//路径下面的每个文件的绝对路径集合，如：D:\GraduationProject\Vega\models\8.10\test\positionthe0phi0force500,500,500.txt
	std::vector<std::string> m_FilePathList;
	//形变文件的集合
	std::vector<Common::SFileFrames> m_FilesData;
	std::vector<Common::SConnectedFemFiles> m_AllReallyLoadConnectedFem;
	//可以看作是一个obj的model对象，有mesh集合，以及group组集合
	CModelDeformationTransform* m_ModelTransformStruct;

	//需要匹配的某些体素下的Object顶点
	std::vector<std::vector<int>> m_CorrectDeformationIndex;

	//第一个是文件的SpKVF的段索引号4，9，14，19，第二个值是当前索引下力的序列
	std::vector<std::pair<int, std::vector<int>>> m_ForceSequence;
	//spKVF文件的受力方向
	std::vector<std::pair<int, std::vector<Common::SForceDirection>>> m_WindDirectionSequence;
	//SpKVF的段索引号，当前索引下K矩阵
	std::vector<std::pair<int, std::vector<std::vector<double>>* >> m_KMartixSequence;
	//SpKVF的段索引号，当前索引下v矩阵
	std::vector<std::pair<int, std::vector<glm::vec3>* >> m_VelocitySequence;
	//SpKVF的段索引号，当前索引下f内矩阵
	std::vector<std::pair<int, std::vector<glm::vec3>* >> m_InternalForcesSequence;
	//所有文件的SpKVF数据索引
	std::vector<int> m_reorderSpKVFSegmentIndexSequence;
	std::vector<std::vector<int>> m_sortSpKVFSegmentIndexSequenceByAngle;
	
	std::vector<std::vector<int>> m_MultipleFramesIndex;
	std::vector<Common::SpKVFData> m_TempSpKVFData;
	std::vector<Common::SpKVFData> m_ZeroKVFData;
	std::vector<int> m_CurrentFrameIndex;
	std::vector<int> m_Flag;

	std::vector<std::vector<std::pair<int, int>>> m_MultipleFileAndFramesIndex;

	int CurrentFrameIndex = 0;

	std::vector<double> m_VelocityRangeError;
	std::vector<std::vector<double>> m_KMatrixRangeError;
	std::vector<double> m_InternalForcesError;
};