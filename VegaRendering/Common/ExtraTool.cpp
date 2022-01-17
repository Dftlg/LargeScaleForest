
#include "ExtraTool.h"

#pragma optimize("",off)

int MaxElement(std::vector<int> &vDataSet)
{
	int max=*(std::max_element(vDataSet.begin(), vDataSet.end()));
	return max;
}

int MinElement(std::vector<int> &vDataSet)
{
	int min=*(std::min_element(vDataSet.begin(), vDataSet.end()));
	return min;
}

bool isDataExist(int number, std::vector<int> &vDataSet)
{
	for (auto tempdata : vDataSet)
		if (tempdata == number)
			return true;
	return false;
}

//改成0-360
int RandomGenerate()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	int randomNumber = rand();
	//return (randomNumber % 360);
	if (randomNumber % 2 == 0) return (randomNumber % 10) * 1;
	return (randomNumber % 10)*-1;
}

glm::vec3 GenerateRamdomScale()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	int randomNumber = rand();
	float scalenumber = randomNumber % 1000 / (float)1000;
	while (scalenumber < 0.6)
	{
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		srand(seed.QuadPart);
		int randomNumber = rand();
		scalenumber = randomNumber % 1000 / (float)1000;
	}
	return glm::vec3(scalenumber, scalenumber, scalenumber);
}

std::vector<std::pair<double,double>> RandomTreePositionGenerate(int vTreeNumber)
{
	std::vector<std::pair<int, int>> tempTreePosition;
	std::vector<std::pair<double, double>> tempTreedoublePosition(vTreeNumber);
	while (tempTreePosition.size()!=vTreeNumber)
	{
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		srand(seed.QuadPart);
		std::pair<int, int> temppair(rand() % vTreeNumber, rand() % vTreeNumber);
		tempTreePosition.push_back(temppair);

		//std::sort(tempTreePosition.begin(), tempTreePosition.end());
		tempTreePosition.erase(std::unique(tempTreePosition.begin(), tempTreePosition.end()), tempTreePosition.end());
	}
	for (auto i = 0; i < tempTreePosition.size(); i++)
	{
		std::default_random_engine e;
		LARGE_INTEGER seed;
		QueryPerformanceFrequency(&seed);
		QueryPerformanceCounter(&seed);
		e.seed(seed.QuadPart);
		std::uniform_real_distribution<double> u(0, 1);
		tempTreedoublePosition[i].first = tempTreePosition[i].first + u(e);
		tempTreedoublePosition[i].second = tempTreePosition[i].second + u(e);
	}
	return tempTreedoublePosition;
}

std::vector<std::pair<double, double>> StableTreePositionGenerate(int vTreesNumber)
{
    /*std::vector<std::pair<double, double>> tempTreedoublePosition(vTreesNumber);
	tempTreedoublePosition[0]=  (std::make_pair(0, 0)  );
	tempTreedoublePosition[1] = (std::make_pair(-20, 0));
	tempTreedoublePosition[2] = (std::make_pair(-40, 0));
	tempTreedoublePosition[3] = (std::make_pair(20, 0) );
	tempTreedoublePosition[4] = (std::make_pair(40, 0) );
	for (int i = 1; i <2 ; i++)
	{
		
		tempTreedoublePosition[i* 5 +  0  ] = (std::make_pair(0, i*(-15)));
		tempTreedoublePosition[i * 5 + 1] = (std::make_pair(-20, i* (-15)));
		tempTreedoublePosition[i * 5 + 2] = (std::make_pair(-40, i*(-15)));
		tempTreedoublePosition[i * 5 + 3] = (std::make_pair(20, i*(-15)));
		tempTreedoublePosition[i * 5 + 4] = (std::make_pair(40, i*(-15)));
		
	}*/

	std::vector<std::pair<double, double>> tempTreedoublePosition(vTreesNumber);
	for (int i = 0; i < vTreesNumber; i++)
	{
		tempTreedoublePosition.push_back(std::make_pair(0, vTreesNumber));
	}


   /* for (int i = 0; i < vTreesNumber; i++)
    {
        tempTreedoublePosition[i]=(std::make_pair(0, i*20));
    }*/

	
    return tempTreedoublePosition;
}

//采样频率，振幅，频率，相位，偏距
//为了不进行大的改动，对采样和搜索两个方法，都假设默认使用GenerateSamplingForece但实际施加的风场力使用新版距离风场作为外力输入。
//（也就是用这个GenerateSamplingForce生成的力作为一种key值，但实际施加的力为距离风场）

std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, float vFrequency, double vPhase,int vYpluse,int wavelength)
{
	double angle = 0.0;
	std::vector<int> tempForces;
	for (int i = 0; i < vSize; i++)
	{
		tempForces.push_back(int(vAmplitude * cos(angle*vFrequency + vPhase * M_PI) + vYpluse));
		angle += (wavelength*M_PI) / vSize;
	}
	return tempForces;
}

double GenerateSamlplingForceWithTime(double time,int vAmplitude, float vFrequency, double vPhase, int vYpluse)
{
    return vAmplitude * cos(2 * M_PI*vFrequency*time + vPhase) + vYpluse;
}



std::vector<int> LineSamplineForce(std::pair<int, int>& vfirstPoint, std::pair<int, int>& vSecondPoint)
{
    std::vector<int> tempYForce;
    int k = (vSecondPoint.second - vfirstPoint.second) / (vSecondPoint.first - vfirstPoint.first);
    for (int xNumber = 0; xNumber < vSecondPoint.first - vfirstPoint.first; xNumber++)
    {
        tempYForce.push_back(k*xNumber + vfirstPoint.second);
    }
    return tempYForce;
}

std::vector<int> GenerateLineForce(int vFrameNumber,std::vector<std::pair<int,int>>& vForceConfig)
{
    if (vForceConfig.back().first != vFrameNumber)
    {
        vForceConfig.push_back(std::make_pair(vFrameNumber, vForceConfig[vForceConfig.size()-1].second));
    }
    std::vector<int> tempForce;
    for (int i = 0; i < vForceConfig.size()-1; i++)
    {
        std::vector<int> temp=LineSamplineForce(vForceConfig[i], vForceConfig[i+1]);
        tempForce.insert(tempForce.end(), temp.begin(), temp.end());
    }
    return tempForce;
}

std::vector<std::vector<double>> GetForceConfigurate(const std::string& vFilePath,const std::string &vExternFile,int &vTheta,int &vPhi, std::vector<std::pair<int, int>>& voLineForceConfig)
{
	std::vector<std::string> ForceConfig;
	std::vector <std::vector<double>> tempConfig;
	int thetaPos = vFilePath.find("the");
	int phiPos = vFilePath.find("phi");
	int forcePos = vFilePath.find("force");
	int IndexPos = vFilePath.find("Index");
	int endPos = vFilePath.find(".");
	vTheta = std::stoi(vFilePath.substr(thetaPos + 3, phiPos - thetaPos - 3));
	vPhi = std::stoi(vFilePath.substr(phiPos + 3, forcePos - phiPos - 3));
	int vforceNumber = std::stoi(vFilePath.substr(forcePos + 5, IndexPos - forcePos - 5));

	std::string forceFileName = vExternFile +"/"+ vFilePath.substr(IndexPos + 5, endPos - IndexPos - 5) + ".txt";

    bool isLineForce=false;

	std::ifstream positionFile(forceFileName, std::ios::in);
	std::string lineString;
	while (getline(positionFile, lineString))
	{
        if (lineString == "initForce:")
        {
            isLineForce = true;
            break;
        }
		boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
		std::vector<double> tempCon;
		std::vector<std::string>::iterator it;
		for (it = ForceConfig.begin(); it != ForceConfig.end(); ++it)
		{
			tempCon.push_back(std::stof(*it));
		}
		tempConfig.push_back(tempCon);
	}
    if (isLineForce == true)
    {
        getline(positionFile, lineString);
        voLineForceConfig.push_back(std::make_pair(0, std::stoi(lineString)));
        getline(positionFile, lineString);
        while (getline(positionFile, lineString))
        {
            boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
            std::pair<int, int> tempPair(std::stoi(ForceConfig[0]), std::stoi(ForceConfig[1]));
            voLineForceConfig.push_back(tempPair);
        }
    }
	return tempConfig;
	
}

//std::vector<std::pair<int,int>> GetLineForceConfigurate(const std::string &vExternFile)
//{
//    std::vector<std::string> ForceConfig;
//    std::vector <std::pair<int,int>> tempConfig;
//
//    std::ifstream positionFile(vExternFile, std::ios::in);
//    std::string lineString;
//    getline(positionFile, lineString);
//    if (lineString == "initForce:")
//    {
//        getline(positionFile, lineString);
//        tempConfig.push_back(std::make_pair(0, std::stoi(lineString)));
//    }
//    getline(positionFile, lineString);
//    while (getline(positionFile, lineString))
//    {
//        boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
//        std::pair<int, int> tempPair(std::stoi(ForceConfig[0]), std::stoi(ForceConfig[1]));
//        tempConfig.push_back(tempPair);
//    }
//    return tempConfig;
//
//}

void GetStemAndLeafStreePointsConfigurate(const std::string& vFilePath, int& vStemPullvertexNums, std::pair<std::vector<int>, std::vector<float>>&vStreePointAndScale,std::vector<std::pair<int,int>>& vForceDeviationDirection,std::vector<float>&vFrequency,std::vector<float>&vPhase)
{
    std::pair<std::vector<int>, std::vector<float>> temp;
    std::vector<int> tempStreePoints;
    std::vector<float> tempStreePointsScales;
    std::vector<std::string> StreePointsConfig;
    std::ifstream positionFile(vFilePath, std::ios::in);
    std::string lineString;
	getline(positionFile, lineString);
	if (lineString == "scale:")
	{
		getline(positionFile, lineString);
		boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
		std::vector<std::string>::iterator it;
		for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
		{
			tempStreePointsScales.push_back(std::stof(*it));
		}
	}
	StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "stem:")
    {
        getline(positionFile, lineString);
        boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
        std::vector<std::string>::iterator it;
        for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
        {
            tempStreePoints.push_back(std::stoi(*it));
        }
    }
	vStemPullvertexNums = tempStreePoints.size();
    StreePointsConfig.clear();
    getline(positionFile, lineString);
    if (lineString == "leaf:")
    {
        getline(positionFile, lineString);
        if (lineString != "")
        {
            boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
            std::vector<std::string>::iterator it;
            for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
            {
                tempStreePoints.push_back(std::stoi(*it));
            }
			getline(positionFile, lineString);
			if (lineString == "leafFrequency:")
			{
				getline(positionFile, lineString);
				boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
				std::vector<std::string>::iterator it;
				for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
				{
					vFrequency.push_back(std::stof(*it));
				}
			}
			StreePointsConfig.clear();
			getline(positionFile, lineString);
			if (lineString == "leafPhase:")
			{
				getline(positionFile, lineString);
				boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
				std::vector<std::string>::iterator it;
				for (it = StreePointsConfig.begin(); it != StreePointsConfig.end(); ++it)
				{
					vPhase.push_back(std::stof(*it));
				}
			}
			StreePointsConfig.clear();
			getline(positionFile, lineString);
			if (lineString == "leafDeviationDirection:")
			{
				getline(positionFile, lineString);
				boost::split(StreePointsConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
				for (int i = 0; i < StreePointsConfig.size() / 2; i++)
				{
					std::pair<int, int> tempPair;
					tempPair.first = std::stoi(StreePointsConfig[2 * i]);
					tempPair.second = std::stoi(StreePointsConfig[2 * i + 1]);
					vForceDeviationDirection.push_back(tempPair);
				}
			}
        }    
    }
       
    StreePointsConfig.clear();   
    if (tempStreePoints.size() != tempStreePointsScales.size())
        std::cout << "pullVertex GetStemAndLeafStreePointsConfigurate doesn't match" << std::endl;
    else
    {
        vStreePointAndScale.first = tempStreePoints;
        vStreePointAndScale.second = tempStreePointsScales;
    }
}



//输入数据，控制范围在数据的周围每次扩大10例如：10,100,0.1,0.001，控制其范围的倍率
double OneNumberRangeError(float vNumber,int vControlFloatPosition,int vRange)
{
	float tempNumber = abs(vNumber);
	int BaseNumber = 0;
	if (tempNumber < 1)
	{
		while (tempNumber < 1)
		{
			tempNumber *= 10;
			BaseNumber++;
		}
		return (1.0 / pow(10,(BaseNumber + vControlFloatPosition)))*vRange;
	}
	else if (tempNumber >= 1)
	{
		while (tempNumber >= 1)
		{
			tempNumber /= 10;
			BaseNumber++;
		}
		return (pow(10,(BaseNumber-1 - vControlFloatPosition)))*vRange;
	} 
}

void TransformCartesianCorrdinate2SphericalCorrdinate(glm::vec3 &vStartPoint, glm::vec3 &vEndPoint, double &voRadius, int& voTheta, int& voPhi)
{
    glm::vec3 tempVector =glm::normalize(vEndPoint - vStartPoint);
    glm::vec3 tempXYAxisChange(tempVector.x, tempVector.z, tempVector.y);
    voRadius = sqrt(tempXYAxisChange.x*tempXYAxisChange.x + tempXYAxisChange.y*tempXYAxisChange.y + tempXYAxisChange.z*tempXYAxisChange.z);
    double ThetaRadian = acos(tempVector.z / voRadius);
    double PhiRadian = atan(tempVector.y / tempVector.x);

    voTheta = 180 * ThetaRadian / M_PI;
    voPhi = 180 * PhiRadian / M_PI;
}

double Point2LineDistance(Common::SXYZLine& vLine, glm::vec3 &Point)
{
    double distance = abs(vLine.A*Point.x + vLine.B*Point.y + vLine.C) / sqrt(pow(vLine.A, 2) + pow(vLine.B, 2));
    return distance;
}

void SetWorldConSyForce(float vforceDegree, double vforceDirectionTheta, double vforceDirectionPhi, double voforce[3])
{
    double Thetaradian = vforceDirectionTheta * M_PI / 180;
    double Phiradian = vforceDirectionPhi * M_PI / 180;
    voforce[0] = cos(Thetaradian)*cos(Phiradian)*vforceDegree;
    voforce[1] = sin(Thetaradian) * vforceDegree;
    voforce[2] = cos(Thetaradian)*sin(Phiradian)*vforceDegree;
}

#pragma optimize("",on)
