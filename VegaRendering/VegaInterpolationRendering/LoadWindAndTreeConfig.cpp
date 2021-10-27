#include "LoadWindAndTreeConfig.h"

CLoadWindAndTreeConfig::CLoadWindAndTreeConfig(int vTreeNumber, const std::string & vFilePath)
{
    m_TreeNumber = vTreeNumber;
	m_FilePath = vFilePath;
    readTreeParaConfig(vFilePath);
    productMultipleTreeWindPara();
    __productEachFormNumberArray();
}

void CLoadWindAndTreeConfig::readTreeParaConfig(const std::string &vFilePath)
{
    std::ifstream positionFile(vFilePath);
    std::string lineString;
    while (getline(positionFile,lineString))
    {
        std::istringstream sin(lineString);
        std::string str;
        sin >> str;
        if (str == "TreeNumber")
        {
            getline(positionFile, lineString);
            int TreeNumber = atoi(lineString.c_str());
            m_EachTypeTreeNumber.push_back(TreeNumber);
        }
        else if (str == "TreeRotationAngle")
        {
            getline(positionFile, lineString);
            int Angle = atoi(lineString.c_str());
            m_TreeRotationAngle.push_back(Angle);
        }
        else if(str=="WindPara")
        {
            getline(positionFile, lineString);
            int ParaNumber = atoi(lineString.c_str());
            std::vector<SWaveFunctionPara> tempWavePara;
            for (int i = 0; i < ParaNumber; i++)
            {
                std::vector<std::string> ForceConfig;
                getline(positionFile, lineString);
                boost::split(ForceConfig, lineString, boost::is_any_of(","), boost::token_compress_off);
                SWaveFunctionPara oneWavePara(ForceConfig);
                tempWavePara.push_back(oneWavePara);
            }
            m_MultipleTreePara.push_back(tempWavePara);
        }
    }
}

void CLoadWindAndTreeConfig::productMultipleTreeWindPara()
{
    int tempFileConfigTreeNumber=0;
    for (auto i : m_EachTypeTreeNumber)
    {
        tempFileConfigTreeNumber += i;
    }
	if (tempFileConfigTreeNumber != m_TreeNumber)
	{
		std::cout << "------------------------" << std::endl;
		std::cout << "TreeFileConfig TreeNumber Error" << std::endl;
		std::cout << "you should change " << m_FilePath << " TreeNumber" << std::endl;
	}
    else
    {
        for (int i = 0; i < m_EachTypeTreeNumber.size(); i++)
        {
            for (int k = 0; k < m_EachTypeTreeNumber[i]; k++)
            {
                m_ProductRotationAngle.push_back(m_TreeRotationAngle[i]);
                //m_ProductMultipleTreePara.push_back(m_MultipleTreePara[i]);
            }
        }
    }
}

std::vector<Common::SForceDirection> CLoadWindAndTreeConfig::getTreeRotationAngle()
{
    std::vector<Common::SForceDirection> vectempForceDirection;
    for (int i = 0; i < m_TreeRotationAngle.size(); i++)
    {
        Common::SForceDirection tempForceDirection(0, m_TreeRotationAngle[i]);
        vectempForceDirection.push_back(tempForceDirection);
    }
    return vectempForceDirection;
}

void CLoadWindAndTreeConfig::__productEachFormNumberArray()
{
    int sum = 0;
    for (int i = 0; i < m_EachTypeTreeNumber.size(); i++)
    {
        sum += m_EachTypeTreeNumber[i];
        m_TypeOfTreeEachFormNumber.push_back(sum);
    }
}
