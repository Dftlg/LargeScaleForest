#pragma once
//#include <glm/vec3.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <io.h>
#include <assimp/mesh.h>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "ExtraTool.h"
#include "common.h"
#include <fstream>
#include<sstream>

struct SWaveFunctionPara
{
	//振幅
	int Amplitude;
	
	//频率
	int Frequency;

	//相位
	double Phase;
	
	//偏距
	int Ypluse;

    //是否进行脉冲函数处理
    int Pulse;

    //对与脉冲函数中下方的函数直线进行上下的浮动调整
    int DropNumber;

	SWaveFunctionPara() = default;
	SWaveFunctionPara(int vAmplitude, int vFrequency, double vPhase, int vYpluse,int vPulse=0,int vDropNumber=0)
	{
		Amplitude = vAmplitude;
		Frequency = vFrequency;
		Phase = vPhase;
		Ypluse = vYpluse;
        Pulse = vPulse;
        DropNumber = vDropNumber;
	}
    SWaveFunctionPara(std::vector<std::string> &vPara)
    {
        Amplitude = atoi(vPara[0].c_str());
        Frequency = atoi(vPara[1].c_str());
        Phase = atof(vPara[2].c_str());
        Ypluse = atoi(vPara[3].c_str());
        Pulse = 0;
        DropNumber = 0;
        if (vPara.size() == 5)
        {
            Pulse = atoi(vPara[4].c_str());
        }
        else if(vPara.size()==6)
        {
            Pulse = atoi(vPara[4].c_str());
            DropNumber= atoi(vPara[5].c_str());
        }
    }
};

class CWindField
{
public:
	CWindField()=default;
	//direction wind
    //vSwavePara为每一棵树独有的风场函数
    //最后一个参数为场景中的风场方向
	CWindField(const int vSize,const std::vector<SWaveFunctionPara> vSwavePara,const int vWavelength,Common::SForceDirection vWindDirection,Common::SForceDirection vTreeRotationDirection);
	//specific wind source
    //初始化生成风源
    //vSwavePara为风源的风场函数,vWindCenter风源中心，vSize,vWaveLength,生成的风场函数帧数，Sphere4Influence风源影响范围，vWindCenterMovePosition风源每个阶段移动的位置，vWindCenterMoveFrames风源在一个阶段内移动所需要帧数
	CWindField(const glm::vec3 vWindCenter, const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength,double Sphere4Influence,std::vector<glm::vec3>& vWindCenterMovePosition,std::vector<int>& vWindCenterMoveFrames,std::vector<double> & vMoveScale);
	~CWindField()=default;

    //setTreePositionAndRotationAngle In Sence
    void setTreePositionAndRotationAngle(std::vector<glm::vec3>& vTreePosition,std::vector<Common::SForceDirection>& vTreeRotationDirection);

    void caculateWindForcesAndDirection2Trees();

    void caculateWindForcesAndDirection2OneTree(int vTreeIndex);

    Common::SForceDirection caculateRelativeDirection(Common::SForceDirection &vWindDirection, Common::SForceDirection &vTreeRotationDirection);
	std::vector<int> getDirectionWindForces() { return m_Forces[0]; };
	std::vector<Common::SForceDirection> getDirectionWindDirection() { return m_RelativeDirectionOfWindAndTree[0]; };

	std::vector<int> getSpecificWindSourceForces(glm::vec3 vObjectPosition);

    void saveForces2File(const std::string filePath);

private:

    //frameindex can get windsource force and windsource position
    int __caculateWindForceOnTree(int vFrameIndex,glm::vec3& vTreePosition);

	glm::vec3 m_WindCenter;
	double m_WindInfluenceField;
	std::vector<SWaveFunctionPara> m_WavePara;
	int AmplitudeWindCenter;

    //size Common::ProductFrameNumber
    std::vector<glm::vec3> m_WindSourceCenterMovePosition;

    std::vector<glm::vec3> m_WindSourceCenterMoveVelocity;
    std::vector<int> m_WindCenterMoveFrames;

    std::vector<glm::vec3> m_TreesPosition;
    std::vector<Common::SForceDirection> m_TreesRotationAngle;
    std::vector<int> m_WindCenterForces;

    //定向风源
	std::vector<std::vector<int>> m_Forces;
	std::vector<std::vector<Common::SForceDirection>> m_RelativeDirectionOfWindAndTree;
};
