#include "VegaFemFactory.h"
#include <time.h>

#pragma optimize("",off)

CVegaFemFactory::CVegaFemFactory(const std::string & vDirectoryName, const std::string & vMutilVerticesBaseFile,const std::string &vCorrectDeformationUVertexIndex)
{
	//m_FilesData中每个对象的文件名和句对路径加上
	readFilePath4Directory(vDirectoryName);
	//根据上面解析的文件名将m_FilesData每个对象的角度和波动序列加上
	setDeformationStateFromFileName();
	//可以看作是一个obj的model对象，有mesh集合，以及group组集合，
	m_ModelTransformStruct = new CModelDeformationTransform(vMutilVerticesBaseFile);
	//加载需要判断的位移索引
	//readCorrectUdeformationIndex(vCorrectDeformationUVertexIndex);
}

//将文件夹下所有文件创建多个SFileFrames文件对象，但并未加载数据，只是将文件对应的名字和绝对路径加上
void CVegaFemFactory::readFilePath4Directory(const std::string & vDirectoryName)
{
	intptr_t  hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(vDirectoryName).append("\\*.txt").c_str(), &fileinfo)) != -1)
	{
		do
		{
			m_FilePathList.push_back(p.assign(vDirectoryName).append("\\").append(fileinfo.name));

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	assert(m_FilePathList.size() > 0);

	for (auto i = 0; i < m_FilePathList.size(); i++)
	{
		m_FilesData.push_back(Common::SFileFrames(getFileName(m_FilePathList[i]), m_FilePathList[i]));
	}
	std::cout << "Load Multiple SFileFrames File:" << m_FilePathList.size() << std::endl;

}

void CVegaFemFactory::write2File(const std::string & vPath, std::vector <std::pair<int, double>>& vOutputData, std::vector <std::pair<int, double>>& vForceFrame, std::vector <std::pair<int, double>>& vKVfFrame,int vFrameIndex)
{
	std::ofstream outFile;
	outFile.open(vPath, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	outFile<<"/////////////////////"<<std::endl;
	outFile << "Frame" << vFrameIndex << std::endl;
	for (int i = 0; i < vOutputData.size(); i++)
	{
		outFile << vOutputData[i].first << ",    ";
		outFile << vOutputData[i].second <<",    "<< vForceFrame[i].second<<",     "<<vKVfFrame[i].second<<std::endl;
	}
	outFile.close();
}

void CVegaFemFactory::writeFindFrameIndex2File(const std::string & vPath, std::pair<int, int>& vTempTreeFileAndFrameIndex)
{
	std::ofstream outFile;
	outFile.open(vPath, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	outFile << vTempTreeFileAndFrameIndex.first << " " << vTempTreeFileAndFrameIndex.second << std::endl;
}


//对于搜索出的SFileFrames加载数据
void CVegaFemFactory::readFramesDeformationData(std::vector<Common::SFileFrames>& vSearchFrames, int vSearchConnectionIndex)
{
	Common::SConnectedFemFiles tempConnectedFile(vSearchConnectionIndex);
	//记录所给角度下的位移需要几个位移文件来插值
	int counterConnectFileNumber = 0;
	//搜索到的指定角度下相关的位移文件集合
	time_t current = time(NULL);
	for (auto searchindex = 0; searchindex < vSearchFrames.size(); searchindex++)
	{
		//for循环路径下所有的位移文件
		for (auto fileIndex = 0; fileIndex < m_FilesData.size(); fileIndex++)
		{
			if (vSearchFrames[searchindex].FileName == m_FilesData[fileIndex].FileName)
			{
				//readDeformationDataByMutileThread(m_FilesData[fileIndex], m_FilesData[fileIndex].FilePath, fileIndex);
				
				readKVFFileData(m_FilesData[fileIndex].FilePath, m_FilesData[fileIndex]);
				//readUdeformationData(m_FilesData[fileIndex].FilePath, m_FilesData[fileIndex]);

				int timeStepCount = 0;
				std::ifstream positionFile(m_FilesData[fileIndex].FilePath);
				std::string lineString;
				char s[4096];
				double position[3];
				if (!positionFile.is_open())
				{
					std::cout << "Error: could not open vertex file" << m_FilesData[fileIndex].FilePath << std::endl;
				}
				int Frameindex = 1;
				
				FILE *fileDir = fopen(m_FilesData[fileIndex].FilePath.c_str(), "rb");
				if(fileDir==NULL)
				{
					perror("read_file fopen");
					return;
				}
				//std::vector<glm::vec3>Udata(35132);//Udata[0]
                int verticesNum = m_ModelTransformStruct->getVerticesNumber();
				Common::SFileData singFrame;
				singFrame.BaseFileDeformations.resize(verticesNum);
				clock_t start1 = clock();
				while (fread(reinterpret_cast<char*>(&singFrame.BaseFileDeformations[0]), sizeof(glm::vec3) * verticesNum, 1, fileDir))
				{
					m_FilesData[fileIndex].Frames.push_back(singFrame);
				}
				clock_t end1 = clock();
				std::cout << "time is : " << (double)(end1 - start1) / CLOCKS_PER_SEC << "s" << std::endl;
				//关闭文件
				m_FilesData[fileIndex].isLoadDataSet = true;
				tempConnectedFile.FemDataset.push_back(&m_FilesData[fileIndex]);
				counterConnectFileNumber++;
			}
		}
	}
	time_t last = time(NULL);
	std::cout<<"time is"<<last-current<<std::endl;
	if (counterConnectFileNumber == 1) tempConnectedFile.Type = Common::EFileFramesType::OneRelatedFile;
	else if (counterConnectFileNumber == 2) tempConnectedFile.Type = Common::EFileFramesType::TwoRelatedFiles;
	else if (counterConnectFileNumber == 4) tempConnectedFile.Type = Common::EFileFramesType::FourRelatedFiles;

	m_AllReallyLoadConnectedFem.push_back(tempConnectedFile);

	std::cout << "Finish Load Search FileData To ConnectedFileFrames" << std::endl;
}

//read frames deformation based files index sequences,return matched frames data
//void CVegaFemFactory::readFramesDeformationDataBasedFilesIndex(std::vector<std::pair<int, int>>&vFilesAndFramesIndexSequence, std::vector<std::vector<glm::vec3>> &voMatchedFramesData)
//{
//	std::vector<int>tempIndexSequece;
//	for (int i = 0; i < vFilesAndFramesIndexSequence.size(); i++)
//	{
//		tempIndexSequece.push_back(vFilesAndFramesIndexSequence[i].first);
//	}
//	sort(tempIndexSequece.begin(), tempIndexSequece.end());
//	tempIndexSequece.erase(std::unique(tempIndexSequece.begin(), tempIndexSequece.end()), tempIndexSequece.end());
//	Common::SConnectedFemFiles tempConnectedFile(0);
//	for (int i = 0; i < tempIndexSequece.size(); i++)
//	{
//		time_t current = time(NULL);
//		int timeStepCount = 0;
//		std::ifstream positionFile(m_FilesData[tempIndexSequece[i]].FilePath);
//		std::string lineString;
//		char s[4096];
//		double position[3];
//		if (!positionFile.is_open())
//		{
//			std::cout << "Error: could not open vertex file" << m_FilesData[tempIndexSequece[i]].FilePath << std::endl;
//		}
//		int Frameindex = 0;
//		int stop = 0;
//		while (getline(positionFile, lineString))
//		{
//			sprintf(s, "Position%d", timeStepCount);
//			std::istringstream sin(lineString);
//			std::string str;
//			sin >> str;//Position%04d后面有空格
//			//一帧
//			if (str == s)
//			{
//				std::string VertexSizeStr;
//				getline(positionFile, VertexSizeStr);
//				int VertexSize = atoi(VertexSizeStr.c_str());
//
//				getline(positionFile, lineString);
//				std::istringstream dataset(lineString);
//
//				//一个位移文件数据中的一帧顶点位移
//				Common::SFileData tempFileData(Frameindex);
//				for (int j = 0; j < VertexSize; j++)
//				{
//					dataset >> position[0] >> position[1] >> position[2];
//					tempFileData.BaseFileDeformations.push_back(glm::vec3(position[0], position[1], position[2]));
//				}
//				timeStepCount++;
//				Frameindex++;
//				m_FilesData[tempIndexSequece[i]].Frames.push_back(tempFileData);
//			}
//		}
//		m_FilesData[tempIndexSequece[i]].isLoadDataSet = true;
//		//tempConnectedFile.FemDataset.push_back(&m_FilesData[tempIndexSequece[i]]);
//		//m_AllReallyLoadConnectedFem[tempIndexSequece[i]] = tempConnectedFile;
//		time_t last = time(NULL);
//		std::cout << "the time of load " << tempIndexSequece[i] << " is" << last - current << std::endl;
//	}
//
//	for (int i = 0; i < vFilesAndFramesIndexSequence.size(); i++)
//	{
//		//每20帧进行一次形变矫正
//		//if (i%4 ==0||i!=0 )
//		//{
//		//	voMatchedFramesData
//		//}
//		for(int k=4;k>=0;k--)
//			voMatchedFramesData.push_back(m_AllReallyLoadConnectedFem[vFilesAndFramesIndexSequence[i].first].FemDataset[0]->Frames[vFilesAndFramesIndexSequence[i].second - k].BaseFileDeformations);
//
//	}
//}

void CVegaFemFactory::readKVFFileData(const std::string & vFile, Common::SFileFrames & vFileFrame)
{
    const size_t last_slash_idx = vFile.rfind('.txt');
    std::string FramesKVFFileName = vFile.substr(0, last_slash_idx - 3);
    FramesKVFFileName = FramesKVFFileName + ".spkvf";
    std::ifstream KVFFile(FramesKVFFileName);
    std::string lineString;
    if (!KVFFile.is_open())
    {
        std::cout << "Error: could not open vertex file" << FramesKVFFileName << std::endl;
        return;
    }
    double position[3];
    while (getline(KVFFile, lineString))
    {
        getline(KVFFile, lineString);
        int ElementNumber = atoi(lineString.c_str()) * 8 * 3;
        Common::SpKVFData tempKVFData;

        Common::SForceDirection tempWindDirection(vFileFrame.Theta, vFileFrame.Phi);
        for (int i = 0; i < 5; i++)
        {
            tempKVFData.WindDirection.push_back(tempWindDirection);
        }
        getline(KVFFile, lineString);
        if (lineString == "FrameIndex")
        {
            getline(KVFFile, lineString);
            tempKVFData.FrameIndex = atoi(lineString.c_str());
        }
        getline(KVFFile, lineString);
        if (lineString == "Force")
        {
            getline(KVFFile, lineString);
            std::istringstream ForceSet(lineString);
            std::vector<int> tempForces;
            for (auto k = 0; k < Common::ForcesSampling; k++)
            {
                double tempKnumber;
                ForceSet >> tempKnumber;
                tempForces.push_back(tempKnumber);
            }
            tempKVFData.Forces = tempForces;
        }
        getline(KVFFile, lineString);
        if (lineString == "Kmatrix")
        {
            getline(KVFFile, lineString);
            int kMatrixLines = atoi(lineString.c_str());
            for (auto i = 0; i < kMatrixLines; i++)
            {
                getline(KVFFile, lineString);
                std::istringstream DataSet(lineString);
                int KmatrixOneLineNumber;
                DataSet >> KmatrixOneLineNumber;
                tempKVFData.KLengths.push_back(KmatrixOneLineNumber);
                std::vector<double> tempKNumbers;
                for (auto k = 0; k < KmatrixOneLineNumber; k++)
                {
                    double tempKnumber;
                    DataSet >> tempKnumber;
                    tempKNumbers.push_back(tempKnumber);
                }
                tempKVFData.Kmatrix.push_back(tempKNumbers);
            }
        }
        getline(KVFFile, lineString);
        if (lineString == "internalForces")
        {
            getline(KVFFile, lineString);
            int internalForceNumbers = atoi(lineString.c_str());
            for (auto i = 0; i < internalForceNumbers; i++)
            {
                getline(KVFFile, lineString);
                std::istringstream InterFace(lineString);
                double temp;
                InterFace >> temp;
                for (int j = 0; j < 3; j++)
                {
                    InterFace >> position[0] >> position[1] >> position[2];
                }
                tempKVFData.InternalForces.push_back(glm::vec3(position[0], position[1], position[2]));
            }
        }
        getline(KVFFile, lineString);
        if (lineString == "velocity")
        {
            getline(KVFFile, lineString);
            int velocityNumbers = atoi(lineString.c_str());
            for (auto i = 0; i < velocityNumbers; i++)
            {
                getline(KVFFile, lineString);
                std::istringstream Velocity(lineString);
                double temp;
                Velocity >> temp;
                for (int j = 0; j < 3; j++)
                {
                    Velocity >> position[0] >> position[1] >> position[2];
                }
                tempKVFData.Velocity.push_back(glm::vec3(position[0], position[1], position[2]));
            }
        }
        vFileFrame.KVFFrameDatas.push_back(tempKVFData);
    }
}

//多线程读取一个文件
//第二个参数已经加载的结构体的第几个FileFrames
void CVegaFemFactory::readDeformationDataByMutileThread(Common::SFileFrames & vBaseFileFramesStruct, const std::string & vFilePath, int vSFileFramesIndex)
{
	std::vector<long long> BlockSizes;
	__getFileSeekDirOfEachBlock(vFilePath, BlockSizes);

	std::vector<Common::SFileData> Frames;
	for (int i = 0; i < Common::readFrames; i++)
	{
		Common::SFileData tempFileData(i);
		Frames.push_back(tempFileData);
	}

	//int i = 2;
	//readDeformationDataInOneThread(Frames[i], i, vFilePath, BlockSize);
	std::vector<std::thread> ReadFrameThreads;
	for (int i = 0; i < Common::readFrames; i++)
	{
		Common::SFileData& temp = Frames[i];
		std::thread th(&CVegaFemFactory::readDeformationDataInOneThread, this, std::ref(Frames[i]), i, vFilePath, BlockSizes[i]);
		ReadFrameThreads.emplace_back(std::move(th));
	}

	auto it = ReadFrameThreads.begin();
	for (; it != ReadFrameThreads.end(); ++it)
		(*it).join();

	std::this_thread::sleep_for(std::chrono::seconds(3));
	vBaseFileFramesStruct.Frames = Frames;
}

void CVegaFemFactory::readDeformationDataInOneThread(Common::SFileData & vFileData, int vTid, const std::string & vFilePath, long long vBlockSize)
{
	std::ifstream positionFile(vFilePath.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "Thread :" << vTid << "failed to Open" << std::endl;
		return;
	}
	std::cout << "vTid" << vTid * vBlockSize << std::endl;
	positionFile.seekg(vBlockSize);
	char PositionIndex[4096];
	double position[3];
	std::string lineString;

	sprintf(PositionIndex, "Position%04d", vTid + 1);
	getline(positionFile, lineString);
	std::istringstream sin(lineString);
	std::string FilePositonIndex;
	sin >> FilePositonIndex;
	if (FilePositonIndex == PositionIndex)
	{
		std::cout << FilePositonIndex << std::endl;
		std::string VertexSizeStr;
		getline(positionFile, VertexSizeStr);
		int VertexSize = atoi(VertexSizeStr.c_str());

		getline(positionFile, lineString);
		std::istringstream Dataset(lineString);

		for (int j = 0; j < VertexSize; j++)
		{
			Dataset >> position[0] >> position[1] >> position[2];
			vFileData.BaseFileDeformations.push_back(glm::vec3(position[0], position[1], position[2]));
		}
	}
}

void CVegaFemFactory::addSeekgOfEachFramesBlock(const std::string & vFilePath)
{
	const size_t last_slash_idx = vFilePath.rfind('.txt');
	std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + "Block.block";
	std::ifstream positionFile(vFilePath.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "failed to Open File" << vFilePath << "Can't get File Para" << std::endl;
		return;
	}
	std::vector<long long>tempBlockSizes;
	tempBlockSizes.push_back(0);
	std::string lineString;
	while (getline(positionFile, lineString))
	{
		getline(positionFile, lineString);
		getline(positionFile, lineString);
		long long BlockSize = positionFile.tellg();
		tempBlockSizes.push_back(BlockSize);
	}
	positionFile.close();

	std::ofstream writePositionFile(FramesBlockFileName, std::ios::out);
	for (auto i = 0; i < tempBlockSizes.size(); i++)
	{
		writePositionFile << tempBlockSizes[i] << " ";
	}
	writePositionFile << std::endl;
	writePositionFile.close();
}

void CVegaFemFactory::getDisplacementTrajectory(std::vector<std::vector<glm::vec3>>& PositionsDeformation)
{
	//m_MultipleFileAndFramesIndex[vTreeIndex][vFrameIndex]
	/*for (int i = 0; i < m_MultipleFileAndFramesIndex[0].size(); i++)
	{
		int fileIndex = m_MultipleFileAndFramesIndex[0][i] / Common::SamplingFrameNumber;
	}*/
}

//对于已经创建的多个SFileFrames对象，根据每个对象的文件名解析出其对应的两个角度以及力的波动序列，进行对象的填充，此时还是没有读入每帧的位移数据
void CVegaFemFactory::setDeformationStateFromFileName()
{
	int thetaPos;
	int phiPos;
	int forcePos;
	int IndexPos;
	for (auto i = 0; i < m_FilesData.size(); i++)
	{
		std::vector<std::string> ForceSequence;
		thetaPos = m_FilesData[i].FileName.find("the");
		phiPos = m_FilesData[i].FileName.find("phi");
		forcePos = m_FilesData[i].FileName.find("force");
		IndexPos = m_FilesData[i].FileName.find("Index");
		
		m_FilesData[i].Theta = std::stoi(m_FilesData[i].FileName.substr(thetaPos + 3, phiPos - thetaPos - 3));
		m_FilesData[i].Phi = std::stoi(m_FilesData[i].FileName.substr(phiPos + 3, forcePos - phiPos - 3));

		int tempForceNumber = std::stoi(m_FilesData[i].FileName.substr(forcePos + 5, IndexPos - forcePos - 5));
		//std::cout << tempForceNumber<<std::endl;
		std::string tempForceDirectory = m_FilesData[i].FilePath.substr(0,m_FilesData[i].FilePath.find_last_of("/"));
		std::string tempFileForceName = tempForceDirectory +"/EachFileForce/"+ m_FilesData[i].FileName.substr(IndexPos + 5) + ".txt";
		std::ifstream positionFile(tempFileForceName, std::ios::in);
		std::string lineString;
		int forcenumber = 0;
        bool isLineForce = false;
		while (getline(positionFile, lineString))
		{
            if (lineString == "initForce:")
            {
                isLineForce = true;
                break;
            }
			boost::split(ForceSequence, lineString, boost::is_any_of(","), boost::token_compress_off);
			std::vector<double> tempCon;
			std::vector<std::string>::iterator it;
			std::vector<double> tempSequence;
			for (it = ForceSequence.begin(); it != ForceSequence.end(); ++it)
			{
				tempSequence.push_back(std::stof(*it));
			}
			m_FilesData[i].ForceFluctuationSequence.push_back(tempSequence);
			tempSequence.clear();
			forcenumber++;
		}
        if (isLineForce == true)
        {
            getline(positionFile, lineString);
            m_FilesData[i].PolyLineForceSequence.push_back(std::make_pair(0, std::stoi(lineString)));
            getline(positionFile, lineString);
            while (getline(positionFile, lineString))
            {
                boost::split(ForceSequence, lineString, boost::is_any_of(","), boost::token_compress_off);
                std::pair<int, int> tempPair(std::stoi(ForceSequence[0]), std::stoi(ForceSequence[1]));
                m_FilesData[i].PolyLineForceSequence.push_back(tempPair);
            }
        }
	}
}

std::string CVegaFemFactory::getFileName(const std::string & vFileDirectory)
{
	int firstPos = vFileDirectory.find_last_of("\\");
	int secondPos = vFileDirectory.find_last_of(".");
	std::string fileName = vFileDirectory.substr(firstPos + 1, secondPos - firstPos - 1);
	return fileName;
}

//将模型的原始形变数据扩大。
//返回值用新的变量是否可以用引用，感觉不行，插值出的不能再SFileData中找到。
//vConnectionIndex代表了第i棵树，vTimestep代表了第几帧
std::vector<Common::SFileDataGroup> CVegaFemFactory::getConnectedFemMutileDeformation(int vConnectionIndex, int vTimestep)
{
	//搜索所有已经存储的相关Connection项,得出每一棵树位移所需要的相关位移文件
	for (int connectIndex = 0; connectIndex < m_AllReallyLoadConnectedFem.size(); connectIndex++)
	{
		//找到了第vConnectionIndex所需要的插值位移文件
		if (vConnectionIndex == m_AllReallyLoadConnectedFem[connectIndex].ConnectedIndex)
		{
			//这里就暂定插值文件就是本身，即不需要通过其它文件来插值
			if (m_AllReallyLoadConnectedFem[connectIndex].Type == Common::EFileFramesType::OneRelatedFile)
			{
				//对一个位移文件中的所有帧数进行循环
				for (int time = 0; time < (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames.size(); time++)
				{
					//找到当前要绘制的帧数
					if ((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FrameIndex == vTimestep)
					{
						//返回模型的增量
						m_ModelTransformStruct->ConvertVertex2mutileVerteices((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time]);

						return (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FileDeformation;

					}
				}
				std::cout << "Finish Load Multile Vertex" << std::endl;
			}
		}


	}
}

void CVegaFemFactory::cleanSFileDataGroup(int vConnectionIndex, int vTimestep)
{
	for (int connectIndex = 0; connectIndex < m_AllReallyLoadConnectedFem.size(); connectIndex++)
	{
		if (vConnectionIndex == m_AllReallyLoadConnectedFem[connectIndex].ConnectedIndex)
		{
			for (int time = 0; time < (m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames.size(); time++)
			{
				if ((m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FrameIndex == vTimestep)
				{
					(m_AllReallyLoadConnectedFem[connectIndex].FemDataset[0])->Frames[time].FileDeformation.clear();
				}
			}
		}
	}
}

void CVegaFemFactory::__getFileSeekDirOfEachBlock(const std::string & vFilePath, std::vector<long long>& vBlock)
{
	const size_t last_slash_idx = vFilePath.rfind('.txt');
	std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + "Block.block";

	std::ifstream positionFile(FramesBlockFileName.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "failed to Open File" << vFilePath << "Can't get File Para" << std::endl;
		addSeekgOfEachFramesBlock(vFilePath);
		std::cout << "create Data Block" << std::endl;
	}
	positionFile.close();

	std::ifstream preparepositionFile(FramesBlockFileName.c_str(), std::ios::in);
	std::string lineString;
	getline(preparepositionFile, lineString);
	std::istringstream Dataset(lineString);
	for (int i = 0; i < Common::readFrames; i++)
	{
		long long tempData;
		Dataset >> tempData;
		vBlock.push_back(tempData);
	}
}

//
////在已经加载FileIndex但尚未加载位移数据的SFileFrames所有搜索相关的SFileFrames并返回
////返回临近的多个方向数据索引标识
//std::vector<Common::SFileFrames> CVegaFemFactory::searchFileFramesOnAnimation(const int vTheta, const int vPhi, const std::vector<double>& vForceFluctuationSequence)
//{
//	int ThetaIndex = vTheta / 30;
//	int PhiIndex = vPhi / 30;
//	int ThetaSecondIndex = ThetaIndex + 1;
//	int PhiSecondIndex = PhiIndex + 1;
//	if (vTheta < 0)
//	{
//		ThetaSecondIndex = ThetaIndex - 1;
//	}
//	if (vPhi < 0)
//	{
//		PhiSecondIndex = PhiIndex - 1;
//	}
//	std::vector<Common::SFileFrames> fileFrames;
//	if (vTheta % 30 == 0 && vPhi % 30 == 0)
//	{
//		for (auto file : m_FilesData)
//		{
//			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//		}
//	}
//	else if (vTheta % 30 == 0 && vPhi % 30 != 0)
//	{
//		for (auto file : m_FilesData)
//		{
//			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//			else if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiSecondIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//		}
//	}
//	else if (vTheta % 30 != 0 && vPhi % 30 == 0)
//	{
//		for (auto file : m_FilesData)
//		{
//			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//		}
//	}
//	else
//	{
//		for (auto file : m_FilesData)
//		{
//			if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//			else if (file.Theta == (ThetaIndex * 30) && file.Phi == (PhiSecondIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//			else if (file.Theta == (ThetaSecondIndex * 30) && file.Phi == (PhiSecondIndex * 30))
//			{
//				if (vForceFluctuationSequence == file.ForceFluctuationSequence)
//				{
//					fileFrames.push_back(file);
//				}
//			}
//		}
//	}
//	return fileFrames;
//}

std::vector<Common::SFileFrames> CVegaFemFactory::searchFileFrameOnAttribute()
{
	std::vector<Common::SFileFrames> fileFrames;
	for (auto file : m_FilesData)
	{
		fileFrames.push_back(file);
	}
	return fileFrames;
}

void CVegaFemFactory::readCorrectUdeformationIndex(const std::string & vFilePath)
{
	std::ifstream positionFile(vFilePath.c_str(), std::ios::in);
	if (!positionFile.good())
	{
		std::cout << "failed to Open File" << vFilePath << "Can't get CorrectDeformationIndex" << std::endl;
	}
	std::string lineString;
	while (getline(positionFile,lineString))
	{
		int tempVertexSize = std::atoi(lineString.c_str());
		std::vector<int> tempVertices;
		for (auto i = 0; i < tempVertexSize; i++)
		{
			getline(positionFile, lineString);
			tempVertices.push_back(std::atoi(lineString.c_str()));
		}
		m_CorrectDeformationIndex.push_back(tempVertices);
		tempVertices.clear();
	}
}

void CVegaFemFactory::searchMatchedDeformationFrames(std::vector<glm::vec3> & vUdeltaDeformation,std::vector<glm::vec3> & vFrameUDeformationData)
{
	std::vector<Common::SMatchedDeformationFrames> MatchedFrames;
	Common::SMatchedDeformationFrames tempMatchedFrame;
	for (auto i = 0; i < m_FilesData.size(); i++)
	{
		//每个文件中10帧的数据形变
		for (auto fileFrameSpDeformation = 0; fileFrameSpDeformation < m_FilesData[i].Deformations.size(); fileFrameSpDeformation++)
		{
			int Counter = 0;
			for (auto vertexInElement = 0; vertexInElement < m_CorrectDeformationIndex.size(); vertexInElement++)
			{
				for (auto objectVertexIndex = 0; objectVertexIndex < m_CorrectDeformationIndex[vertexInElement].size(); objectVertexIndex++)
				{
					//统计顶点
					Counter += distanceError(vFrameUDeformationData[objectVertexIndex], m_FilesData[i].Deformations[fileFrameSpDeformation].Deformation[objectVertexIndex]);
				}
			}
			tempMatchedFrame.CounterNumber = Counter;
			tempMatchedFrame.FileName = m_FilesData[i].FileName;
			tempMatchedFrame.FrameIndex = m_FilesData[i].Deformations[fileFrameSpDeformation].FrameIndex;
			MatchedFrames.push_back(tempMatchedFrame);
		}
	}
	//获取最匹配的形变数据
	std::sort(MatchedFrames.begin(), MatchedFrames.end());
	for (int i = 0; i < m_FilesData.size(); i++)
	{
		if (MatchedFrames[0].FileName == m_FilesData[i].FileName)
		{
			for (int k = 0; k < m_FilesData[i].Deformations.size(); k++)
			{
				if (MatchedFrames[0].FrameIndex == m_FilesData[i].Deformations[k].FrameIndex)
				{
					for (int j = 0; j < m_FilesData[i].Deformations[k].Deformation.size(); j++)
					{
						vUdeltaDeformation[j] = vUdeltaDeformation[j] + (vFrameUDeformationData[j] - m_FilesData[i].Deformations[k].Deformation[j]);
					}
				}
			}
		}
	}
}

void CVegaFemFactory::readUdeformationData(const std::string & vFile, Common::SFileFrames &vFileFrame)
{
	const size_t last_slash_idx = vFile.rfind('.txt');
	std::string FramesSpUdeformation = vFile.substr(0, last_slash_idx - 3);
	FramesSpUdeformation = FramesSpUdeformation + ".spDemn";
	std::ifstream UdeformationFile(FramesSpUdeformation);
	std::string lineString;
	if (!UdeformationFile.is_open())
	{
		std::cout << "Error: could not open SpDeformation file" << FramesSpUdeformation << std::endl;
		return;
	}

	double position[3];
	while (getline(UdeformationFile, lineString))
	{
		const size_t last_idx = lineString.rfind('tion');
		std::string index = lineString.substr(last_idx+1);
		int tempIndex = atoi(index.c_str());

		getline(UdeformationFile, lineString);
		int DeformationVertexNumber = atoi(lineString.c_str());

		getline(UdeformationFile, lineString);
	
		std::istringstream Dataset(lineString);

		//一个位移文件数据中的一帧顶点位移
		std::vector<glm::vec3> tempDeformation;
		double position[3];
		for (int j = 0; j < DeformationVertexNumber; j++)
		{
			Dataset >> position[0] >> position[1] >> position[2];
			tempDeformation.push_back(glm::vec3(position[0], position[1], position[2]));
		}
		vFileFrame.Deformations.push_back(Common::SpDeformation(tempIndex, tempDeformation));
	}
}


void CVegaFemFactory::initMatchedFrameStruct(int vTreeSize)
{
	for (int i = 0; i < m_AllReallyLoadConnectedFem.size(); i++)
	{
		for (int k = 0; k < m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas.size(); k++)
		{
			std::vector<int> tempExternalForces = m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].Forces;
			m_reorderSpKVFSegmentIndexSequence.push_back(m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].FrameIndex + Common::SamplingFrameNumber * i);
			m_ForceSequence.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence.back(), tempExternalForces));
			m_WindDirectionSequence.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence.back(), m_AllReallyLoadConnectedFem[i].FemDataset[0]->KVFFrameDatas[k].WindDirection));
		}

	}
	std::vector<std::vector<double>>* tempKMartix;
	std::vector<glm::vec3>* tempVelocity;
	std::vector<glm::vec3>* tempInternalForces;
	for (int i = 0; i < m_reorderSpKVFSegmentIndexSequence.size(); i++)
	{
		//重组Spkvf文件格式，将其中文件的索引拍成连续的4，9....64...69
		tempKMartix = &m_AllReallyLoadConnectedFem[m_reorderSpKVFSegmentIndexSequence[i] / Common::SamplingFrameNumber].FemDataset[0]->KVFFrameDatas[(m_reorderSpKVFSegmentIndexSequence[i] % Common::SamplingFrameNumber) / 5].Kmatrix;
		m_KMartixSequence.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence[i], tempKMartix));
		tempVelocity = &m_AllReallyLoadConnectedFem[m_reorderSpKVFSegmentIndexSequence[i] / Common::SamplingFrameNumber].FemDataset[0]->KVFFrameDatas[(m_reorderSpKVFSegmentIndexSequence[i] % Common::SamplingFrameNumber) / 5].Velocity;
		m_VelocitySequence.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence[i], tempVelocity));
		tempInternalForces = &m_AllReallyLoadConnectedFem[m_reorderSpKVFSegmentIndexSequence[i] / Common::SamplingFrameNumber].FemDataset[0]->KVFFrameDatas[(m_reorderSpKVFSegmentIndexSequence[i] % Common::SamplingFrameNumber) / 5].InternalForces;
		m_InternalForcesSequence.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence[i], tempInternalForces));
	}

	initTempMultipleTreeData(vTreeSize);
	/*std::fill_n(std::back_inserter(m_Flag), vTreeSize, 1);*/
}

void CVegaFemFactory::initKVFDataSearchRangeError()
{
	Common::SpKVFData firstKVFData4FirstFile = getFirstKVFDataFromFirstFileFrame();
	//compute V RangeError
    for (auto tempv : firstKVFData4FirstFile.Velocity)
    {
        m_VelocityRangeError.push_back(OneNumberRangeError(tempv.x, 1, 5));
    }
		
	for (auto tempF : firstKVFData4FirstFile.InternalForces)
		m_InternalForcesError.push_back(OneNumberRangeError(tempF.x,1,5));
	for (auto tempK : firstKVFData4FirstFile.Kmatrix)
	{
		std::vector<double> tempOneVertexRangeError;
		for (int tempNumber = 0; tempNumber < tempK.size(); tempNumber++)
		{
			tempOneVertexRangeError.push_back(OneNumberRangeError(tempK[tempNumber],1,5));
		}
		m_KMatrixRangeError.push_back(tempOneVertexRangeError);
		tempOneVertexRangeError.clear();
	}
}

void CVegaFemFactory::initTempMultipleTreeData(int vTreeSize)
{
	m_MultipleFramesIndex.resize(vTreeSize);
	m_MultipleFileAndFramesIndex.resize(vTreeSize);
	for (int i = 0; i < vTreeSize; i++)
	{
		m_MultipleFileAndFramesIndex[i].resize(5);
	}
	m_TempSpKVFData.resize(vTreeSize);
	m_ZeroKVFData.resize(vTreeSize);
	m_CurrentFrameIndex.resize(vTreeSize);
	m_Flag.resize(vTreeSize, 1);
}

void CVegaFemFactory::resetTempMultipleTreeData(int vTreeSize)
{
	for (int i = 0; i < vTreeSize; i++)
	{
		m_MultipleFramesIndex[i].assign(5, 0);
		m_MultipleFileAndFramesIndex[i].assign(5, std::pair<int,int>(0,0));
		m_TempSpKVFData[i] = m_ZeroKVFData[i];
		m_CurrentFrameIndex[i] = 0;
		m_Flag[i] = 1;
	}
}

//以5帧为单位进行帧段的查找匹配操作,对于多棵树同时进行查找操作
//每次计算需要当前的一段5力以及下一段的5个力，每次计算出前5段力的结果
//输出当前查找到每颗树5帧的索引号
void CVegaFemFactory::searchMatchedFrameSequences(std::vector<std::vector<int>> &voExtraForces,std::vector<std::vector<Common::SForceDirection>> &vForceDirection)
{
	std::vector<int> FrameIndexSequence;

	
#pragma omp parallel for
		for (int treeIndex = 0; treeIndex < m_TempSpKVFData.size(); treeIndex++)
		{
			m_TempSpKVFData[treeIndex].Forces.clear();
			m_TempSpKVFData[treeIndex].WindDirection.clear();
			//每棵树的当前5个力的段
			for (int k = 0; k < 5; k++)
			{
				m_TempSpKVFData[treeIndex].Forces.push_back(voExtraForces[treeIndex][k]);
				m_TempSpKVFData[treeIndex].WindDirection.push_back(vForceDirection[treeIndex][k]);
			}
			//可以并行
           
            searchMatchedOneTreeFrameSequences(m_MultipleFramesIndex[treeIndex], m_TempSpKVFData[treeIndex], voExtraForces[treeIndex], m_CurrentFrameIndex[treeIndex], m_Flag[treeIndex]);


			
			for (int j = 0; j < 5; j++)
			{
				m_MultipleFileAndFramesIndex[treeIndex][j].first = m_MultipleFramesIndex[treeIndex][j] / Common::SamplingFrameNumber;
				m_MultipleFileAndFramesIndex[treeIndex][j].second = m_MultipleFramesIndex[treeIndex][j] % Common::SamplingFrameNumber;
			}
		}
}

static int number = 0;
static int a;

//voExtraForces需要当前的力以及下一段的力
void CVegaFemFactory::searchMatchedOneTreeFrameSequences(std::vector<int> & voMatchedFrameSequenceIndex, Common::SpKVFData& voSpKVData, std::vector<int>&voExtraForces,int &voCurrentFrameIndex, int& vIsFirstFrame)
{
	std::vector<std::pair<int, double>> tempForceErrorSequence;
	std::vector<std::pair<int, double>> tempKErrorSequence;
	std::vector<std::pair<int, double>> tempVelocityErrorSequence;
	std::vector<std::pair<int, double>> tempInternalForcesErrorSequence;
	std::vector <std::pair<int, double>> gaussianForceErrrorSequence;
	
	//windDirection
	std::vector<int> ThetaData;
	std::vector<int> PhiData;
	for (int i = 0; i < voSpKVData.WindDirection.size(); i++)
	{
		ThetaData.push_back(voSpKVData.WindDirection[i].Theta);
		PhiData.push_back(voSpKVData.WindDirection[i].Phi);
	}
	int MaxTheta = MaxElement(ThetaData)+15;
	int MinTheta = MinElement(ThetaData) - 15;
    
	int MaxPhi = MaxElement(PhiData) + 15;
	int MinPhi = MinElement(PhiData) - 15;

    //std::cout << MaxPhi << "  ||" << MinPhi << std::endl;

	std::vector<int> searchFileAndFrameIndexWithWind;
	for (int i = 0; i < m_WindDirectionSequence.size(); i++)
	{
		if (m_WindDirectionSequence[i].second[0].Theta<MaxTheta&&m_WindDirectionSequence[i].second[0].Theta > MinTheta)
			if (m_WindDirectionSequence[i].second[0].Phi<MaxPhi&&m_WindDirectionSequence[i].second[0].Phi > MinPhi)
				searchFileAndFrameIndexWithWind.push_back(m_WindDirectionSequence[i].first);
	}

	//f外
	for (int i = 0; i < m_ForceSequence.size(); i++)
	{
		int forceError = 0;
		for (auto k = 0; k < 5;k++)
		{
			forceError += AbsError(voSpKVData.Forces[k], m_ForceSequence[i].second[k]);
		}
		forceError /= Common::ExpandForceError;
        if (!isDataExist(m_VelocitySequence[i].first, searchFileAndFrameIndexWithWind))
        {
            forceError = 99999;
        }
		tempForceErrorSequence.push_back(std::make_pair(m_ForceSequence[i].first, forceError));
	}
	//initial tempSpKVData just based on Forces when find the first section
	//第一段直接默认使用相似力的帧段
	//for (int i = 0; i < m_ForceSequence.size(); i++)
	//{
	//	std::cout << tempForceErrorSequence[i].first<<"--"<< tempForceErrorSequence[i].second<< std::endl;
	//}

	if (vIsFirstFrame == 1)
	{
		voMatchedFrameSequenceIndex.clear();
		for (int i = 0; i < tempForceErrorSequence.size(); i++)
		{
            if (tempForceErrorSequence[i].first % (Common::SamplingFrameNumber) > 5 )
			/*if (tempForceErrorSequence[i].first % (Common::SamplingFrameNumber) > 20|| !isDataExist(tempForceErrorSequence[i].first, searchFileAndFrameIndexWithWind))*/
				tempForceErrorSequence[i].second = 99999;
		}
		std::vector<std::pair<int, double>>tempSortedForceSequence = tempForceErrorSequence;
		sort(tempSortedForceSequence.begin(), tempSortedForceSequence.end(), [](const std::pair<int, double>&x, const std::pair<int, double>&y)->double {return x.second < y.second; });
		voCurrentFrameIndex = tempSortedForceSequence[0].first;
		voSpKVData = m_AllReallyLoadConnectedFem[tempSortedForceSequence[0].first / Common::SamplingFrameNumber].FemDataset[0]->KVFFrameDatas[(tempSortedForceSequence[0].first % Common::SamplingFrameNumber) / 5];
		vIsFirstFrame = 0;
		for (int k = 4; k >= 0; k--)
			voMatchedFrameSequenceIndex.push_back(tempSortedForceSequence[0].first - k);
	}
	else
	{
		voMatchedFrameSequenceIndex.clear();
		/*for (int i = 0; i < Common::TreesNumber; i++)
		{
			m_MultipleFileAndFramesIndex[i].resize(5);
		}*/
		//将力权重用高斯权重表示
		double gaussianForceErrrorSum = 0;
		for (int i = 0; i < tempForceErrorSequence.size(); i++)
		{
			//std::cout << tempForceErrorSequence[i].second << std::endl;
			gaussianForceErrrorSequence.push_back(std::make_pair(tempForceErrorSequence[i].first, GaussianFunction(tempForceErrorSequence[i].second / double(4000), sqrt(0.2), 0, 0.001)));
			gaussianForceErrrorSum += gaussianForceErrrorSequence[i].second;
		}

		//sort(gaussianForceErrrorSequence.begin(), gaussianForceErrrorSequence.end(), [](const std::pair<double, double>&x, const std::pair<double, double>&y)->int {return x.second > y.second; });

		/*velocity
		com                                                                                                                                                               pare velocity*/
		for (int i = 0; i < m_VelocitySequence.size(); i++)
		{
			if (!isDataExist(m_VelocitySequence[i].first, searchFileAndFrameIndexWithWind)||gaussianForceErrrorSequence[i].second <0.6)
			{
				tempVelocityErrorSequence.push_back(std::make_pair(m_VelocitySequence[i].first, 0));
				continue;
			}
			int count = 0;
			if (m_VelocitySequence[i].first % Common::SamplingFrameNumber == 4)
			{
				//std::cout << "test" << m_VelocitySequence[i].second->size();
				for (int k = 0; k < m_VelocitySequence[i].second->size(); k++)
				{
					if (AbsError(voSpKVData.Velocity[k], glm::vec3(0, 0, 0), m_VelocityRangeError[k]))
						count++;
				}
				tempVelocityErrorSequence.push_back(std::make_pair(m_VelocitySequence[i].first, count));
				continue;
			}
			for (int k = 0; k < m_VelocitySequence[i].second->size(); k++)
			{
				if (AbsError((*(m_VelocitySequence[i - 1].second))[k], voSpKVData.Velocity[k], m_VelocityRangeError[k]))
					count++;
			}
			tempVelocityErrorSequence.push_back(std::make_pair(m_VelocitySequence[i].first, count));
		}

		//k
	//compare K Martix
        clock_t start1 = clock();
#pragma omp parallel for
		for (int i = 0; i < m_KMartixSequence.size(); i++)
		{
			if (!isDataExist(m_VelocitySequence[i].first, searchFileAndFrameIndexWithWind)||gaussianForceErrrorSequence[i].second <0.6|| tempVelocityErrorSequence[i].second==0)
			{
				tempKErrorSequence.push_back(std::make_pair(m_KMartixSequence[i].first, 0));
				continue;
			}
			int count = 0;
			//数据只存了4，9，14等第0段没存，而第0段的数据是由静止的kvf矩阵计算而成
			if (m_KMartixSequence[i].first % Common::SamplingFrameNumber == 4)
			{
				for (int k = 0; k < m_KMartixSequence[i].second->size(); k++)
				{
					//std::cout << "test" << (*m_KMartixSequence[i].second)[k].size();
					for (int j = 0; j < (*m_KMartixSequence[i].second)[k].size(); j++)
					{
						if (AbsError(voSpKVData.Kmatrix[k][j], 0, m_KMatrixRangeError[k][j]))
							count++;
					}
				}
				tempKErrorSequence.push_back(std::make_pair(m_KMartixSequence[i].first, count));
				continue;
			}
			for (int k = 0; k < m_KMartixSequence[i].second->size(); k++)
			{
				for (int j = 0; j < (*m_KMartixSequence[i].second)[k].size(); j++)
				{
					if (AbsError(((*m_KMartixSequence[i-1].second)[k])[j], voSpKVData.Kmatrix[k][j], m_KMatrixRangeError[k][j]))
						count++;
				}
			}
			tempKErrorSequence.push_back(std::make_pair(m_KMartixSequence[i].first, count));
		}

        clock_t end1 = clock();
        //std::cout << "time is : " << (double)(end1 - start1) / CLOCKS_PER_SEC << "s" << std::endl;

		//compare internalForces
		for (int i = 0; i < m_InternalForcesSequence.size(); i++)
		{
			if (!isDataExist(m_VelocitySequence[i].first, searchFileAndFrameIndexWithWind)||gaussianForceErrrorSequence[i].second <0.6||(tempVelocityErrorSequence[i].second == 0&& tempKErrorSequence[i].second==0))
			{
				tempInternalForcesErrorSequence.push_back(std::make_pair(m_InternalForcesSequence[i].first, 0));
				continue;
			}
			int count = 0;
			if (m_InternalForcesSequence[i].first % Common::SamplingFrameNumber == 4)
			{
				for (int k = 0; k < m_InternalForcesSequence[i].second->size(); k++)
				{
					if (AbsError(voSpKVData.InternalForces[k], glm::vec3(0, 0, 0), Common::internalForceErrorRange, m_InternalForcesError[k]))
						count++;
				}
				tempInternalForcesErrorSequence.push_back(std::make_pair(m_InternalForcesSequence[i].first, count));
				continue;
			}
			for (int k = 0; k < m_InternalForcesSequence[i].second->size(); k++)
			{
				if (AbsError((*(m_InternalForcesSequence[i - 1].second))[k], voSpKVData.InternalForces[k], m_InternalForcesError[k]))
					count++;
			}
			tempInternalForcesErrorSequence.push_back(std::make_pair(m_InternalForcesSequence[i].first, count));
		}

	
		for (int i = 0; i < m_reorderSpKVFSegmentIndexSequence.size(); i++)
		{
			//KMartix weight
			tempKErrorSequence[i].second = tempKErrorSequence[i].second / getKMatrixSumNumber();
			//velocity weight
			tempVelocityErrorSequence[i].second = tempVelocityErrorSequence[i].second / getVeocitySumNumber();
			//internalForce weight
			tempInternalForcesErrorSequence[i].second = tempInternalForcesErrorSequence[i].second / getInternalForceSumNuber();
		}

		//当选择了某一个文件的一帧时会应用该帧对下一帧段的KVF导致下一帧段判断的kvf值一定等于1*kvf权重，特殊判定将这以帧权重比更改
		int NextFrameIndex = voCurrentFrameIndex + 5;

        

		std::vector<std::pair<int, double>> allWeightsSumResults;
		double forcesWeight = 0.75;
		double KVfWeight = 0.25;
#pragma omp parallel for
		for (int i = 0; i < m_reorderSpKVFSegmentIndexSequence.size(); i++)
		{
			double tempResult = 0;
			if (NextFrameIndex == tempVelocityErrorSequence[i].first)
			{
               // tempResult = (gaussianForceErrrorSequence[i].second)*forcesWeight + KVfWeight * (tempKErrorSequence[i].second + tempVelocityErrorSequence[i].second + tempInternalForcesErrorSequence[i].second) / 3;
				tempResult = (gaussianForceErrrorSequence[i].second)*0.8 + 0.2 * (tempKErrorSequence[i].second + tempVelocityErrorSequence[i].second + tempInternalForcesErrorSequence[i].second) / 3;
				//tempResult = 1.0 * (tempKErrorSequence[i].second + tempVelocityErrorSequence[i].second + tempInternalForcesErrorSequence[i].second) / 3;
			}
			else
			{
				tempResult = (gaussianForceErrrorSequence[i].second)*forcesWeight + KVfWeight * (tempKErrorSequence[i].second + tempVelocityErrorSequence[i].second + tempInternalForcesErrorSequence[i].second) / 3;
			}
			allWeightsSumResults.push_back(std::make_pair(m_reorderSpKVFSegmentIndexSequence[i], tempResult));
		}
		
		sort(allWeightsSumResults.begin(), allWeightsSumResults.end(), [](const std::pair<int, double>&x, const std::pair<int, double>&y)->double {return x.second > y.second;});

        //test4.3
        //m_allWeightsSumResults.resize(allWeightsSumResults.size());
        //for (int i = 0; i < allWeightsSumResults.size(); i++)
        //{
        //    m_allWeightsSumResults[i] = allWeightsSumResults[i];
        //}

		voSpKVData = m_AllReallyLoadConnectedFem[allWeightsSumResults[0].first / Common::SamplingFrameNumber].FemDataset[0]->KVFFrameDatas[(allWeightsSumResults[0].first % Common::SamplingFrameNumber) / 5];
		voCurrentFrameIndex = allWeightsSumResults[0].first;
		
		std::vector<std::pair<int, double>>().swap(tempForceErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempKErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempVelocityErrorSequence);
		std::vector<std::pair<int, double>>().swap(tempInternalForcesErrorSequence);

		number++;
		/*std::cout << allWeightsSumResults[0].first / Common::SamplingFrameNumber << "--" << allWeightsSumResults[0].first % Common::SamplingFrameNumber << " ";
		std::cout << std::endl;*/

		for(int k=4;k>=0;k--)
			voMatchedFrameSequenceIndex.push_back(allWeightsSumResults[0].first -k);

	}
	
}

void CVegaFemFactory::saveAllWeightsSumResults(const std::string & vFilePath)
{
    std::ofstream outFile;
    outFile.open(vFilePath, std::ios::in | std::ios::app);
    if (!outFile)
    {
        std::cout << "Can't open the file!!!" << std::endl;
        return;
    }
    for (int i = m_allWeightsSumResults.size()-1; i >=0; i--)
    {
        outFile << m_allWeightsSumResults[i].second << " ";      
    }
    outFile.close();
}

#pragma optimize("",on)
