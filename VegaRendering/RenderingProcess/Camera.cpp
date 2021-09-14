#include "Camera.h"

//m_Front(glm::vec3(1.0f, 0.0f, 1.0f))
CCamera::CCamera(glm::vec3 vPosition, glm::vec3 vUp, float vYaw, float vPitch) : m_Front(glm::vec3(1.0f, 0.0f, 1.0f)), m_MovementSpeed(0.03f), m_MouseSensitivity(0.02f), m_Zoom(45.0f)
{
	m_Position = vPosition;
	m_WorldUp = vUp;
	m_Yaw = vYaw;
	m_Pitch = vPitch;

	__updateCameraVectors();
}

CCamera::CCamera(float vPosX, float vPosY, float vPosZ, float vUpX, float vUpY, float vUpZ, float vYaw, float vPitch) : m_Front(glm::vec3(0.0f, -1.0f, 0.0f)), m_MovementSpeed(0.2f), m_MouseSensitivity(0.02f), m_Zoom(45.0f)
{
	m_Position = glm::vec3(vPosX, vPosY, vPosZ);
	m_WorldUp = glm::vec3(vUpX, vUpY, vUpZ);
	m_Yaw = vYaw;
	m_Pitch = vPitch;
	__updateCameraVectors();
}

//****************************************************************************************************
//FUNCTION:
void CCamera::processKeyboard(ECamera_Movement vDirection, float vDeltaTime)
{
	const float Velocity = m_MovementSpeed * vDeltaTime * 20;

	if (vDirection == FORWARD)
		m_Position += m_Front * Velocity;
	if (vDirection == BACKWARD)
		m_Position -= m_Front * Velocity;
	if (vDirection == LEFT)
		m_Position -= m_Right * Velocity;
	if (vDirection == RIGHT)
		m_Position += m_Right * Velocity;
}

//****************************************************************************************************
//FUNCTION:
void CCamera::processMouseMovement(float vXOffset, float vYOffset, GLboolean vConstrainPitch)
{
	vXOffset *= m_MouseSensitivity;
	vYOffset *= m_MouseSensitivity;

	m_Yaw += vXOffset;
	m_Pitch += vYOffset;

	if (vConstrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}
	__updateCameraVectors();
}

//****************************************************************************************************
//FUNCTION:
void CCamera::processMouseScroll(float vYOffset)
{
	if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
		m_Zoom -= vYOffset;
	if (m_Zoom <= 1.0f)
		m_Zoom = 1.0f;
	if (m_Zoom >= 45.0f)
		m_Zoom = 45.0f;
}

//****************************************************************************************************
//FUNCTION:
void CCamera::__updateCameraVectors()
{
	glm::vec3 Front;
	Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	Front.y = sin(glm::radians(m_Pitch));
	Front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(Front);
	//std::cout << "Camera:" << " " << m_Position.x << " " << m_Position.y << " " << m_Position.z << std::endl;
	//std::cout << "m_Front" << m_Front.x << " " << m_Front.y << " " << m_Front.z << std::endl;
	/*m_Front = glm::vec3(0.0111246, - 0.999848 ,- 0.0134473);
	m_Front = glm::vec3(-0.549614, 0.0321085 ,- 0.834801);*/

	//Camera: 4.03613 0.892339 4.5147
	//m_Front - 0.549614 0.0321085 - 0.834801
	//m_Front = glm::vec3(-0.484646, - 0.347156, 0.587509);
	/*Camera: 15.6695 6.51048 - 12.5966
	m_Front - 0.78498 - 0.351518 0.510138*/
	//-0.551845 0.0369925 - 0.833126
	//yellow
	//m_Front = glm::vec3(-0.549614, 0.0321085, -0.834801);
	//mini
	//m_Front = glm::vec3(-0.274153 ,0.0324574, - 0.961138);
	//apricot
	//m_Front = glm::vec3(-0.272648 ,0.109387, - 0.955875);
	//1:50
	//m_Front = glm::vec3(0.00912665, - 0.249705, - 0.968279);
	//
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	if (startsaveData == true)
	{
		m_PositionList.push_back(m_Position);
		m_FrontList.push_back(m_Front);
	}
}

void CCamera::outputfile(const char* vPath)
{
	std::ofstream outFile;
	outFile.open(vPath, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	for (int i = 0; i < m_PositionList.size(); i++)
	{
		outFile << m_PositionList[i].x << " " << m_PositionList[i].y << " " << m_PositionList[i].z << std::endl;
		outFile << m_FrontList[i].x << " " << m_FrontList[i].y << " " << m_FrontList[i].z << std::endl;
	}
	outFile.close();
}

void CCamera::readCameraPositonFile(const char* vPath)
{
	std::ifstream positionFile(vPath);
	std::string lineString;
	char s[4096];
	double position[3];
	double front[3];
	if (!positionFile.is_open())
	{
		std::cout << "Error: could not Camera File << std::endl;";
	}
	while (getline(positionFile, lineString))
	{
		std::istringstream dataset(lineString);
		dataset >> position[0] >> position[1] >> position[2];
		m_PositionList.push_back(glm::vec3(position[0], position[1], position[2]));
		getline(positionFile, lineString);
		std::istringstream frontset(lineString);
		frontset >> front[0] >> front[1] >> front[2];
		m_FrontList.push_back(glm::vec3(front[0], front[1], front[2]));
	}
		/*stop++;*/
}

void CCamera::UpdataCameraPosition(int vframeIndex)
{
	m_Position = m_PositionList[vframeIndex];
	m_Front = m_FrontList[vframeIndex];
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void CCamera::Positionfile(const char* vPath)
{
	
		
		
	glm::vec3 position1=glm::vec3(15.6695, 6.51048 ,- 12.5966); 
	glm::vec3 position2=glm::vec3(15.6695, 6.51048, - 12.5966);
	glm::vec3 front1=glm::vec3(-0.930089, -0.338081, -0.143653);
	glm::vec3 front2=glm::vec3(-0.484646 ,- 0.347156, 0.587509);
	int frame=500;
	std::ofstream outFile;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> fronts;
	outFile.open(vPath, std::ios::in | std::ios::app);
	if (!outFile)
	{
		std::cout << "Can't open the file!!!" << std::endl;
		return;
	}
	glm::vec3 lengthposition=position2-position1;
	glm::vec3 positionstep = glm::vec3(lengthposition.x / frame, lengthposition.y / frame, lengthposition.z / frame);
	glm::vec3 lengthfront = front2 - front1;
	glm::vec3 frontstep = glm::vec3(lengthfront.x / frame, lengthfront.y / frame, lengthfront.z / frame);
	for (int i = 0; i < frame; i++)
	{
		glm::vec3 temppositonstep(positionstep.x*i, positionstep.y*i, positionstep.z*i);
		glm::vec3 tempfrontstep(frontstep.x*i, frontstep.y*i, frontstep.z*i);
		positions.push_back(position1 + temppositonstep);
		fronts.push_back(front1 + tempfrontstep);
	}
	for (int i = 0; i < frame; i++)
	{
		outFile << positions[i].x << " " << positions[i].y << " " << positions[i].z << std::endl;
		outFile << fronts[i].x << " " << fronts[i].y << " " << fronts[i].z << std::endl;
	}
	outFile.close();
}