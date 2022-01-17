#include"WindFiled.h"

//旧版风场时针对每棵树木施加一个定向的外力，但只是针对模型的几个点
//为了防止大改，现在考虑对于查找过程中的风场，因为是依靠外力参数来搜索，实际上不了解具体风场的关系，只知道每个文件kvf中f是一个波动函数，因此在采样过程中的风场用
//用论文中风场代替，但存储到kvf中时依然使用较为简单f波动函数去存储，搜索也用较为简单的外力进行搜索。而且因为考虑到了风吹树木的方向这个也进行了存储所有不用方向问题


CWindField::CWindField(const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength, Common::SForceDirection vWindDirection, Common::SForceDirection vTreeRotationDirection)
{
	std::vector<int>tempForces(vSize,0);
	for (auto tempwavepara : vSwavePara)
	{
        std::vector<int> temp = GenerateSamplingForce(vSize, tempwavepara.Amplitude, tempwavepara.Frequency, tempwavepara.Phase, tempwavepara.Ypluse, vWavelength);
        if (tempwavepara.Pulse == 1)
        {
            for (int i = 0; i < vSize; i++)
            {
                if (temp[i] < tempwavepara.Ypluse)
                {
                    temp[i] = tempwavepara.Ypluse;
                    if (tempwavepara.DropNumber != 0)
                    {
                        temp[i] -= tempwavepara.DropNumber;
                    }
                }
            }       
        }	
		for (int i = 0; i < vSize; i++)
		{
			tempForces[i] += temp[i];
		}
	}
    Common::SForceDirection tempDirection = caculateRelativeDirection(vWindDirection, vTreeRotationDirection);
	std::vector<Common::SForceDirection> tempDirections(vSize, tempDirection);
	m_Forces.push_back(tempForces);
    m_RelativeDirectionOfWindAndTree.push_back(tempDirections);
}

CWindField::CWindField(const glm::vec3 vWindCenter, const int vSize, const std::vector<SWaveFunctionPara> vSwavePara, const int vWavelength, double Sphere4Influence, std::vector<glm::vec3>& vWindCenterMoveVelocity, std::vector<int>& vWindCenterMoveFrames, std::vector<double> & vMoveScale)
{
    m_WindCenter = vWindCenter;
    m_WindInfluenceField = Sphere4Influence;
    m_WindSourceCenterMoveVelocity = vWindCenterMoveVelocity;
    m_WindCenterMoveFrames = vWindCenterMoveFrames;
    std::vector<int>tempForces(vSize, 0);
    for (auto tempwavepara : vSwavePara)
    {
        std::vector<int> temp = GenerateSamplingForce(vSize, tempwavepara.Amplitude, tempwavepara.Frequency, tempwavepara.Phase, tempwavepara.Ypluse, vWavelength);
        if (tempwavepara.Pulse == 1)
        {
            for (int i = 0; i < vSize; i++)
            {
                if (temp[i] < tempwavepara.Ypluse)
                {
                    temp[i] = tempwavepara.Ypluse;
                    if (tempwavepara.DropNumber != 0)
                    {
                        temp[i] -= tempwavepara.DropNumber;
                    }
                }
            }
        }
        for (int i = 0; i < vSize; i++)
        {
            tempForces[i] += temp[i];
        }
    }
    m_WindCenterForces = tempForces;

    if (m_WindSourceCenterMoveVelocity.size() != m_WindCenterMoveFrames.size())
        std::cout << "the WindSourceMoveVelocity Size must same as WindCenterMoveFrames" << std::endl;

    int tempFrames=0;
    for (auto i : m_WindCenterMoveFrames)
        tempFrames += i;
    if (tempFrames != Common::ProductFrameNumber)
        std::cout << "the WindCenterMoveFrames must same as Common::ProductFrameNumber" << std::endl;

    glm::vec3 tempWindSourcePosition= m_WindCenter;
    for (int i = 0; i < m_WindSourceCenterMoveVelocity.size(); i++)
    {
        glm::vec3 tempVelocity = m_WindSourceCenterMoveVelocity[i];
        for (int k = 0; k < m_WindCenterMoveFrames[i]; k++)
        {
            tempWindSourcePosition += glm::vec1((k + 1))*tempVelocity;
            m_WindSourceCenterMovePosition.push_back(tempWindSourcePosition);
        }
    }

}

void CWindField::setTreePositionAndRotationAngle(std::vector<glm::vec3>& vTreePosition, std::vector<Common::SForceDirection>& vTreeRotationDirection)
{
    m_TreesPosition = vTreePosition;
    m_TreesRotationAngle = vTreeRotationDirection;
}

void CWindField::caculateWindForcesAndDirection2Trees()
{

}

void CWindField::caculateWindForcesAndDirection2OneTree(int vTreeIndex)
{
    std::vector<int> oneTreeForces;
    std::vector<Common::SForceDirection> oneTreeRelativeDirection;
    for (int i = 0; i < m_WindSourceCenterMovePosition.size(); i++)
    {
        double Radius;
        int Theta, Phi;
        TransformCartesianCorrdinate2SphericalCorrdinate(m_WindSourceCenterMovePosition[i], m_TreesPosition[vTreeIndex], Radius, Theta, Phi);
        Common::SForceDirection tempWindDirection(Theta, Phi);
        Common::SForceDirection tempDirection = caculateRelativeDirection(tempWindDirection, m_TreesRotationAngle[vTreeIndex]);
        oneTreeRelativeDirection.push_back(tempDirection);
        oneTreeForces.push_back(__caculateWindForceOnTree(i, m_TreesPosition[vTreeIndex]));
    }
    m_Forces.push_back(oneTreeForces);
    m_RelativeDirectionOfWindAndTree.push_back(oneTreeRelativeDirection);
}

int CWindField::__caculateWindForceOnTree(int vFrameIndex, glm::vec3& vTreePosition)
{

    glm::vec3 RelativeDistance=m_WindSourceCenterMovePosition[vFrameIndex]-vTreePosition;
    double absRelativeDistance = sqrt(RelativeDistance.x*RelativeDistance.x + RelativeDistance.z*RelativeDistance.z);

    //y=-(1/N)*x+1
    int Force = m_WindCenterForces[vFrameIndex] * (-(absRelativeDistance / m_WindInfluenceField) + 1);
    return Force;
}

Common::SForceDirection CWindField::caculateRelativeDirection(Common::SForceDirection &vWindDirection, Common::SForceDirection &vTreeRotationDirection)
{
    Common::SForceDirection tempDirection(0, 0);
    tempDirection.Phi = vWindDirection.Phi - vTreeRotationDirection.Phi;
    //std::cout << -tempDirection.Phi;
    tempDirection.Phi = (-tempDirection.Phi);
    return tempDirection;
}

void CWindField::saveForces2File(const std::string filePath)
{
    std::ofstream outFile;
    outFile.open(filePath, std::ios::in | std::ios::app);
    if (!outFile)
    {
        std::cout << "Can't open the file!!!" << std::endl;
        return;
    }
  
    for (int i = 0; i < m_Forces.size(); i++)
    {
        outFile << "/////////////////////" << std::endl;
        for (int k = 0; k < m_Forces[i].size(); k++)
        {
            outFile << m_Forces[i][k] << ", ";
            if (k % 5 == 0&&k!=0)
            {
                outFile << "\n";
            }
        }
    }
    outFile.close();
}
