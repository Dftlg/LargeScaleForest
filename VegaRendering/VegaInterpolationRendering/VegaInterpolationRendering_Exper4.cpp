//#include <iostream>
////#define GLFW_EXPOSE_NATIVE_GLX
//#include <GL/glew.h>
//#include<glm/glm.hpp>
//#include<glm/gtc/type_ptr.hpp>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image_write.h>
//#include<glm/gtc/matrix_transform.hpp>
//#include <GLFW/glfw3.h>
//#include <vector>
//#include "../RenderingProcess/Camera.h"
//#include "Shader.h"
//#include "Sence.h"
//#include "Mesh.h"
//#include "VegaFemFactory.h"
//#include "../Common/SynchronisedQueue.h"
//#include "../Common/WindFiled.h"
//#include "LoadWindAndTreeConfig.h"
//#include "InitMultipleTypeTree.h"
//#include "../Common/ExtraTool.h"
//#include "RenderStaticSence.h"
//#include "../RenderingProcess/ComputerShader.h"
//#include "../Common/common.h"
//#include "../RenderingProcess/CubicVeg.h"
//
////#define GLFW_EXPOSE_NATIVE_GLX
//
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//#include <stdio.h>
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//
//#ifdef _DEBUG
//#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif
//
//inline void EnableMemLeakCheck()
//{
//	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
//}
//
//
//#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
//#pragma comment(lib, "legacy_stdio_definitions")
//#endif
////
////static void glfw_error_callback(int error, const char* description)
////{
////	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
////}
//
//void write2File();
//void renderPlane(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId, const unsigned int& vTextureOpacityId);
//void renderTree(CShader& vShader, CSence& vModel);
//void renderGrass(CShader & vShader, CSence& vModel);
//void renderLight(CShader& vShader, const unsigned int& VAOId);
//void renderSkybox(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId);
//bool initWindow(GLFWwindow*& vWindow, int vScreenWidth, int vScreenHeight);
//void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset);
//void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos);
//void framebufferSizeCallback(GLFWwindow* vWindow, int vWidth, int vHeight);
//void processInput(GLFWwindow* vWindow);
//int captureScreen2Img(const std::string& vFileName, int vQuality);
//unsigned int loadTexture(char const * path);
//unsigned int loadCubemap(std::vector<std::string> faces);
//
////2560 1440
////1920 1080  960 1080
////1000,1000
//const unsigned int SCR_WIDTH = 1920;
//const unsigned int SCR_HEIGHT = 1080;
//bool shadows = true;
//bool shadowsKeyPressed = false;
//bool automoveCamera = false;
//bool MouseShow = false;
//
//bool startsaveCameraPara = false;
//// camera 0 0.6 1
////0, 4.0, 0
////CCamera Camera(glm::vec3(0, 0.6, 1));
////CCamera Camera(glm::vec3(15.6695, 6.51048, -12.5966));
//
////CCamera Camera(glm::vec3(15.6695, 6.51048, - 12.5966));
////2.339 0.528229 2.1349
////mini
////CCamera Camera(glm::vec3(0.554545 ,0.502902, 0.622372));
////apricot
////CCamera Camera(glm::vec3(0.800835 ,0.434919 ,1.31922));
////yellow
////CCamera Camera(glm::vec3(2.339, 0.528229, 2.1349));
////CCamera Camera(glm::vec3(4.03613 ,0.892339, 4.5147));
//
////1:50tree 
////Camera:
//
//
////
////CCamera Camera(glm::vec3(-0.272438, 2.64901 ,4.87207));
//CCamera Camera(glm::vec3(0.026077, 0.669017, 1.610339));
//float LastX = SCR_WIDTH / 2.0f;
//float LastY = SCR_HEIGHT / 2.0f;
//bool FirstMouse = true;
//
//// timing
//float DeltaTime = 0.0f;
//float LastFrame = 0.0f;
//
////wind�����Ż�
//int windactive = 0;
//float grasstime = 0.0f;
//
//SynchronisedQueue<std::vector<std::pair<int, int>>> SearchQueue[Common::TreesTypeNumber];
//
//std::vector<std::vector<int>> EachFormNumberArray;
//
//int i = 0;
//int Size = 0;
//int FrameNumber = 0;
//////each time change
//int SearchFrameNumber[Common::TreesTypeNumber] = { 0 };
//int SearchFrameStep[Common::TreesTypeNumber] = { 0 };
////int SearchFrameNumber[Common::TreesTypeNumber] = { 0,0,0 };
////int SearchFrameStep[Common::TreesTypeNumber] = { 0,0,0 };
////int SearchFrameNumber[Common::TreesTypeNumber] = { 0,0 };
////int SearchFrameStep[Common::TreesTypeNumber] = { 0,0 };
//
//int ALLTreeNumber = 3;
//
//float bendScale[] = { 0.3,0.2,0.5 };
//float primaryOffsetScale[] = { 0.1,0.5,0.1 };
//
//std::string PathString = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/ExperimentResult/outputImagel.bmp";
//
//std::vector<double>timeSum;
//
//int frameIndex = 0;
//
////��Ⱦ VegGroupVoxelColor
//int VoxelGroupIndex = 11;
//
//
////ǰһ��std::vector��ʾƥ�����ĸ�������һ��std::vector��ʾÿһ֡����Ҫ������
////vMultipleExtraForces ��ʾÿһ֡��ķ���ÿ����5֡����������
////vWindDirection ��ʾÿ֡һ����ķ���
//void InsertSearchTreeFrameIndex(CVegaFemFactory &vVFF, CSence vSence, std::vector<std::vector<int>>& vMultipleExtraForces, std::vector<std::vector<Common::SForceDirection>> & vWindDirection, std::vector<int>& vTreesNumberSubjected2SameWind, int vTreeTypeIndex)
//{
//
//	while (true)
//	{
//		//��ǰ12��֡�ν���һ�����û�ȡ5��֡�κ�����
//		/*if (SearchFrameNumber %Size==0)
//		{
//			std::cout << "search reset" << std::endl;
//			vVFF.resetTempMultipleTreeData(vMultipleExtraForces.size());
//			SearchFrameStep = 0;
//		}*/
//
//		if (SearchFrameNumber[vTreeTypeIndex] == Common::ProductFrameNumber)
//		{
//			break;
//		}
//		if (SearchFrameNumber[vTreeTypeIndex] % 5 == 0)
//		{
//			//ÿ5��������һ��ƥ���5֡
//			std::vector<std::vector<int>> tempMultipleFiveForces(vMultipleExtraForces.size());
//			std::vector<std::vector<Common::SForceDirection>> tempMultipleFiveWindDirection(vMultipleExtraForces.size());
//			for (int i = 0; i < vMultipleExtraForces.size(); i++)
//			{
//				for (int k = (SearchFrameStep[vTreeTypeIndex]) * 5; k < (SearchFrameStep[vTreeTypeIndex] + 1) * 5; k++)
//				{
//					//int t=vMultipleExtraForces[i][k];
//					tempMultipleFiveForces[i].push_back(vMultipleExtraForces[i][k]);
//					tempMultipleFiveWindDirection[i].push_back(vWindDirection[i][k]);
//				}
//			}
//			vVFF.searchMatchedFrameSequences(tempMultipleFiveForces, tempMultipleFiveWindDirection);
//			tempMultipleFiveForces.clear();
//			tempMultipleFiveWindDirection.clear();
//			SearchFrameStep[vTreeTypeIndex]++;
//			//std::vector<std::vector<int>> temp = vFem.getMultipleFramesIndex();
//		}
//		/*if (timeSum.size()==30)
//		{
//			double sumValuee = 0;
//			for (int i = 0; i < timeSum.size(); i++)
//			{
//				sumValuee += timeSum[i];
//			}
//			std::cout << "Average time:" << sumValuee / timeSum.size() << std::endl;
//		}*/
//
//
//		//�����ｫ�ظ����������鸴�ƶ��
//		std::vector<std::pair<int, int>> tempTreeFileAndFrameIndex;
//
//		//std::cout << "TreeIndex" << vTreeTypeIndex << ": ";
//
//		for (int DifferentTreeNumber = 0; DifferentTreeNumber < vTreesNumberSubjected2SameWind.size(); DifferentTreeNumber++)
//		{
//			std::pair<int, int> tempOneTreeFileAndFrameIndex = vVFF.getFileAndFrameIndex(DifferentTreeNumber, SearchFrameNumber[vTreeTypeIndex] % 5);
//			//std::cout << tempOneTreeFileAndFrameIndex.first << "--" << tempOneTreeFileAndFrameIndex.second << "||";
//
//			for (int k = 0; k < vTreesNumberSubjected2SameWind[DifferentTreeNumber]; k++)
//			{
//				tempTreeFileAndFrameIndex.push_back(tempOneTreeFileAndFrameIndex);
//			}
//			//std::cout << std::endl;
//		}
//		SearchQueue[vTreeTypeIndex].Enqueue(tempTreeFileAndFrameIndex);
//		SearchFrameNumber[vTreeTypeIndex]++;
//		tempTreeFileAndFrameIndex.clear();
//	}
//}
//
//int main()
//{
//
//	//positionfile
//	//Camera.Positionfile("G:\\GraduationProject\\Camera\\position1.txt");
//	//EnableMemLeakCheck();
//#pragma region initialize and configure glfw
//	//glfwSetErrorCallback(glfw_error_callback);
//	glfwInit();
//	const char* glsl_version = "#version 430";
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_SAMPLES, 4);
//
//	// ------------------------------
//	GLFWwindow* Window = nullptr;
//	if (!initWindow(Window, SCR_WIDTH, SCR_HEIGHT))
//	{
//		return EXIT_FAILURE;
//	}
//	glfwMakeContextCurrent(Window);
//	glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
//	glfwSetCursorPosCallback(Window, mouseCallback);
//	glfwSetScrollCallback(Window, scrollCallback);
//
//	// tell GLFW to capture our mouse
//	//glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	// configure global opengl state
//	// -----------------------------
//	glEnable(GL_MULTISAMPLE);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//	if (Common::renderingVegMesh == true)
//	{
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	}
//
//
//#pragma endregion
//	/////////////////////////////
//#pragma region build and compile shaders
//	CShader ourSkyBoxShader("skybox.vert", "skybox.frag");
//
//
//#pragma endregion
//
//	if (automoveCamera == true)
//	{
//		Camera.readCameraPositonFile("G:\\GraduationProject\\Camera\\position.txt");
//	}
//
//#pragma region plane vertices data
//	float planeVertices[] = {
//		// positions              // normals       // texture Coords 
//		 30.0f, -0.5f,  30.0f,   0.0f, 1.0f, 0.0f,   30.0f, 30.0f,
//		-30.0f, -0.5f,  30.0f,   0.0f, 1.0f, 0.0f,   0.0f, 30.0f,
//		-30.0f, -0.5f, -30.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
//
//		 30.0f, -0.5f,  30.0f,   0.0f, 1.0f, 0.0f,   30.0f, 30.0f,
//		-30.0f, -0.5f, -30.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
//		 30.0f, -0.5f, -30.0f,   0.0f, 1.0f, 0.0f,   30.0f, 0.0f,
//	};
//#pragma endregion
//
//#pragma region skybox vertices data
//	float skyboxVertices[] = {
//		// positions          
//		-1.0f,  1.0f, -1.0f,
//		-1.0f, -1.0f, -1.0f,
//		 1.0f, -1.0f, -1.0f,
//		 1.0f, -1.0f, -1.0f,
//		 1.0f,  1.0f, -1.0f,
//		-1.0f,  1.0f, -1.0f,
//
//		-1.0f, -1.0f,  1.0f,
//		-1.0f, -1.0f, -1.0f,
//		-1.0f,  1.0f, -1.0f,
//		-1.0f,  1.0f, -1.0f,
//		-1.0f,  1.0f,  1.0f,
//		-1.0f, -1.0f,  1.0f,
//
//		 1.0f, -1.0f, -1.0f,
//		 1.0f, -1.0f,  1.0f,
//		 1.0f,  1.0f,  1.0f,
//		 1.0f,  1.0f,  1.0f,
//		 1.0f,  1.0f, -1.0f,
//		 1.0f, -1.0f, -1.0f,
//
//		-1.0f, -1.0f,  1.0f,
//		-1.0f,  1.0f,  1.0f,
//		 1.0f,  1.0f,  1.0f,
//		 1.0f,  1.0f,  1.0f,
//		 1.0f, -1.0f,  1.0f,
//		-1.0f, -1.0f,  1.0f,
//
//		-1.0f,  1.0f, -1.0f,
//		 1.0f,  1.0f, -1.0f,
//		 1.0f,  1.0f,  1.0f,
//		 1.0f,  1.0f,  1.0f,
//		-1.0f,  1.0f,  1.0f,
//		-1.0f,  1.0f, -1.0f,
//
//		-1.0f, -1.0f, -1.0f,
//		-1.0f, -1.0f,  1.0f,
//		 1.0f, -1.0f, -1.0f,
//		 1.0f, -1.0f, -1.0f,
//		-1.0f, -1.0f,  1.0f,
//		 1.0f, -1.0f,  1.0f
//	};
//#pragma endregion
//
//#pragma region lights data
//	glm::vec3 lightVertices[] = {
//		glm::vec3(-3.5f,  4.5f, 1.0f),
//		glm::vec3(3.5f,  4.5f, 1.0f),
//		glm::vec3(-3.5f,  4.0f, 1.0f),
//		glm::vec3(3.5f,  4.0f, 1.0f),
//	};
//
//	glm::vec3 lightColors[] = {
//		glm::vec3(1.0f, 1.0f, 1.0f),
//		glm::vec3(1.0f, 1.0f, 1.0f),
//		glm::vec3(1.0f, 1.0f, 1.0f),
//		glm::vec3(1.0f, 1.0f, 1.0f)
//	};
//
//	float lightPositionsData[] = {
//		-1.5f,  2.5f, 1.0f,  1.0f, 0.0f, 0.0f,
//		1.5f,  2.5f, 1.0f,	1.0f, 1.0f, 1.0f,
//		-1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f,
//		1.5f,  2.5f, 1.0f,	1.0f, 1.0f, 1.0f,
//		1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f,
//		-1.5f,  2.0f, 1.0f,	1.0f, 1.0f, 1.0f
//	};
//
//	glm::vec3 lightPosition[] = {
//		glm::vec3(0.0f,3.5f,0.0f)
//	};
//	glm::vec3 lightPointColors[] = {
//		glm::vec3(1.0f, 1.0f, 1.0f),
//	};
//
//
//#pragma endregion
//
//#pragma region bind light VAO and VBO
//	unsigned int lightVAO, lightPotionVBO;
//	glGenVertexArrays(1, &lightVAO);
//	glGenBuffers(1, &lightPotionVBO);
//	glBindVertexArray(lightVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, lightPotionVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(lightPositionsData), &lightPositionsData, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glBindVertexArray(0);
//#pragma endregion
//
//#pragma region bind plane VAO and VBO
//	// plane VAO
//	unsigned int planeVAO, planeVBO;
//	glGenVertexArrays(1, &planeVAO);
//	glGenBuffers(1, &planeVBO);
//	glBindVertexArray(planeVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glBindVertexArray(0);
//	// plane load textures
//	unsigned int floorTexture = loadTexture("resources/textures/test.jpg");
//	unsigned int opacityTexture = loadTexture("resources/textures/opacity.png");
//#pragma endregion
//
//
//	glEnable(GL_CULL_FACE);
//
//#pragma region skybox VAO and VBO and Texture
//	// skybox VAO
//	unsigned int skyboxVAO, skyboxVBO;
//	glGenVertexArrays(1, &skyboxVAO);
//	glGenBuffers(1, &skyboxVBO);
//	glBindVertexArray(skyboxVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//
//	std::vector<std::string> faces;
//	//Each time change*************
//	if (Common::renderingTerrainOrWhiltScence == true)
//	{
//		faces.push_back("resources/textures/sky/FluffballDayLeft.png");
//		faces.push_back("resources/textures/sky/FluffballDayRight.png");
//		faces.push_back("resources/textures/sky/FluffballDayTop.png");
//		faces.push_back("resources/textures/sky/FluffballDayBottom.png");
//		faces.push_back("resources/textures/sky/FluffballDayFront.png");
//		faces.push_back("resources/textures/sky/FluffballDayBack.png");
//	}
//	else
//	{
//		faces.push_back("resources/textures/whitesky/white.png");
//		faces.push_back("resources/textures/whitesky/white.png");
//		faces.push_back("resources/textures/whitesky/white.png");
//		faces.push_back("resources/textures/whitesky/white.png");
//		faces.push_back("resources/textures/whitesky/white.png");
//		faces.push_back("resources/textures/whitesky/white.png");
//	}
//	//End Each time change*************
//	//***************
//	/**/
//	//***************
//
//
//	// skybox load textures
//	unsigned int cubemapTexture = loadCubemap(faces);
//	ourSkyBoxShader.use();
//	ourSkyBoxShader.setInt("skybox", 0);
//#pragma endregion
//
//	/////////////////parpall shadow
//	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
//	unsigned int depthMapFBO;
//	glGenFramebuffers(1, &depthMapFBO);
//	unsigned int depthMap;
//	glGenTextures(1, &depthMap);
//	glBindTexture(GL_TEXTURE_2D, depthMap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//	// attach depth texture as FBO's depth buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	///////////////////////////////
//	glm::mat4 lightProjection, lightView;
//	glm::mat4 lightSpaceMatrix;
//	float near_plane = 1.0f, far_plane = 5.0f;
//
//
//	//Each time change*************
//	if (Common::renderingTerrainOrWhiltScence == true)
//	{
//		lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
//	}
//	else
//	{
//		lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
//	}
//
//	//End Each time change*************
//	//
//	//***************
//
//	lightView = glm::lookAt(lightPosition[0], glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.1));
//	lightSpaceMatrix = lightProjection * lightView;
//
//#pragma region bind static
//	std::vector<float> modelscale;
//
//	CRenderStaticSence RenderStaticSence(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, lightSpaceMatrix, lightPosition[0], lightPointColors[0]);
//
//	//FirstTypeTerrain
//	//Each time change*************
//	if (Common::renderingTerrainOrWhiltScence == true)
//	{
//		RenderStaticSence.loadStaticModel("terrain", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/terrain/Mountains2.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.06f);
//	}
//	else
//	{
//		RenderStaticSence.loadStaticModel("terrain", "G:/model/plane/plane.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(1.0f);
//	}
//
//	//modelscale.push_back(0.06f);
//	//End Each time change*************
//
//	//SecondTypePlane
//	//**********************
//	/**/
//	//********************
//
//	std::vector<float> rotations;
//	std::vector<std::pair<double, double>> transform;
//	std::vector<double> zTransform;
//	//terrain
//	rotations.push_back(0);
//	transform.push_back(std::make_pair(0, 0));
//
//	//Each time change*************
//	if (Common::renderingTerrainOrWhiltScence == true)
//	{
//		zTransform.push_back(-12.0);
//	}
//	else
//	{
//		zTransform.push_back(0.0);
//	}
//	//End Each time change*************
//	//***************
//	//
//	//***************
//
//
//	RenderStaticSence.setTerrain("terrain", zTransform);
//
//	rotations.clear();
//	transform.clear();
//	zTransform.clear();
//
//	RenderStaticSence.initModelShaderPara("terrain");
//	RenderStaticSence.setTerrainHightMesh("terrain", 20, 20, 200, 200);
//	RenderStaticSence.getTerrain("terrain");
//
//	//Each time change*************
//	if (Common::renderingTerrainOrWhiltScence == true)
//	{
//		RenderStaticSence.setTerrainHeightYToZero();
//	}
//	//End Each time change*************
//	//lightsource
//	if (Common::renderingLightSource == true)
//	{
//		RenderStaticSence.loadStaticModel("lightsource", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/sphere/sphere.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("lightsource.vert", "lightsource.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("lightsource", lightPosition[0], 0);
//		//RenderStaticSence.setObjectTransform("lightsource", glm::vec3(0.0f,0.0f,0.0f), 0);
//		RenderStaticSence.initModelShaderPara("lightsource");
//	}
//
//
//	////grass1
//#pragma region grass load
//	if (Common::renderingGrass == true)
//	{
//		//Grass
//		RenderStaticSence.loadStaticModel("greenGrass", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/grassGrass/greenGrass/file.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.009f);
//
//		/*RenderStaticSence.loadStaticModel("hay", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/grassGrass/Hay/file.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		modelscale.push_back(0.005f);
//		typeStaticNumber++;*/
//
//		float grassnumber = 20;
//		for (int i = 0; i < grassnumber; i++)
//		{
//			rotations.push_back(30 * i);
//			zTransform.push_back(0);
//		}
//		transform.push_back(std::make_pair(500, 500));
//		transform.push_back(std::make_pair(3000, 800));
//		transform.push_back(std::make_pair(-2500, 1500));
//		transform.push_back(std::make_pair(-1550, 1500));
//		transform.push_back(std::make_pair(-500, -500));
//		transform.push_back(std::make_pair(-3000, -800));
//		transform.push_back(std::make_pair(-5000, 4000));
//		transform.push_back(std::make_pair(-450, 1500));
//		transform.push_back(std::make_pair(700, -4000));
//		transform.push_back(std::make_pair(-3000, -5800));
//		transform.push_back(std::make_pair(-3000, 6000));
//		transform.push_back(std::make_pair(-550, 5500));
//		transform.push_back(std::make_pair(-5100, -5200));
//		transform.push_back(std::make_pair(-4000, -2300));
//		transform.push_back(std::make_pair(-3000, 5000));
//		transform.push_back(std::make_pair(-250, 4500));
//
//		transform.push_back(std::make_pair(-300, -400));
//		transform.push_back(std::make_pair(-2300, -600));
//		transform.push_back(std::make_pair(-4500, 4700));
//		transform.push_back(std::make_pair(-3500, 2600));
//		RenderStaticSence.setModelInstanceAndTransform("greenGrass", transform, rotations, grassnumber);
//		rotations.clear();
//		transform.clear();
//		zTransform.clear();
//
//
//		//std::vector<std::pair<double, double>> tempTransform = RandomTreePositionGenerate(haynumber);
//		//for (int i = 0; i < haynumber; i++)
//		//{
//		//	transform.push_back(std::make_pair((tempTransform[i].first - haynumber / 2) * 400, (tempTransform[i].second - haynumber / 2) * 400));
//		//}
//		RenderStaticSence.initModelShaderPara("greenGrass");
//		RenderStaticSence.setGrassOnTerrain("greenGrass");
//		RenderStaticSence.updataGrassOnTerrain("greenGrass", 0);
//
//		//RenderStaticSence.initModelShaderPara("hay");
//		//RenderStaticSence.setGrassOnTerrain(i);
//		//RenderStaticSence.updataGrassOnTerrain(i,1);
//	}
//
//#pragma endregion
//	//RenderStaticSence.setModelInstanceAndTransform(2, transform, rotations, haynumber);
//	//rotations.clear();
//	//transform.clear();
//
//
//	std::vector<double> TerrainHeigth;
//	double TerrainX;
//	double TerrainZ;
//	double XDensityScale;
//	double ZDensityScale;
//	int TerrainHeightZDensity;
//	RenderStaticSence.getTerrainPara(TerrainHeigth, TerrainX, TerrainZ, XDensityScale, ZDensityScale, TerrainHeightZDensity);
//
//
//#pragma endregion
//
//
//	//////////////////////////////
//
//
//#pragma region load tree model
//		//������ľ�����λ��
//
//	for (int i = 0; i < Common::TreesTypeNumber; i++)
//	{
//		ALLTreeNumber += Common::TreesNumbers[i];
//	}
//	//CInitMultipleTypeTree MultipleTypeTree(Common::TreesTypeNumber, ALLTreeNumber);
//	////////////////////////////////////////////
//	////MultipleTypeTree.InitShadowCubeMapPara(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, shadowTransforms, lightVertices, lightColors);
//	//MultipleTypeTree.InitShadowMapPara(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, lightSpaceMatrix, lightPosition[0], lightPointColors[0]);
//	//MultipleTypeTree.setTerrainPara(TerrainHeigth, TerrainX, TerrainZ, XDensityScale, ZDensityScale, TerrainHeightZDensity);
//
//	//MultipleTypeTree.InitVegaFemFactory("G:/GraduationProject/yellow_tree/deltaU", "../../models/yellow_tree/tree_last_test.obj", "../../models/yellow_tree/ObjectVertexIndex.txt", 7);
//	//MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "G:/GraduationProject/yellow_tree/WindAndTreeConfig/Config.txt");
//	//MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
//	//MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag");
//	//MultipleTypeTree.InitTreeModel("../../models/yellow_tree/tree_last_test.obj", 0);
//
//	/*MultipleTypeTree.InitVegaFemFactory("G:/GraduationProject/mini_mapleTree/deltaU", "../../models/mini_mapleTree/tree.obj", "../../models/mini_mapleTree/ObjectVertexIndex.txt", 1);
//	MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "G:/GraduationProject/mini_mapleTree/WindAndTreeConfig/Config.txt");
//	MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
//	MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag");
//	MultipleTypeTree.InitTreeModel("../../models/mini_mapleTree/tree.obj", 0);
//*/
///*MultipleTypeTree.InitVegaFemFactory("G:/GraduationProject/mini_mapleTree/deltaU", "../../models/mini_mapleTree/tree.obj", "../../models/mini_mapleTree/ObjectVertexIndex.txt",1);
//MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "G:/GraduationProject/mini_mapleTree/WindAndTreeConfig/Config.txt");
//MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
//MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag", "point_shadows_depth.gs");
//MultipleTypeTree.InitTreeModel("../../models/mini_mapleTree/tree.obj", 0);*/
//
////MultipleTypeTree.InitVegaFemFactory("G:/GraduationProject/apricot_tree/deltaU", "../../models/apricot_tree/tree.obj", "../../models/apricot_tree/ObjectVertexIndex.txt", 1);
////MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "G:/GraduationProject/apricot_tree/WindAndTreeConfig/Config.txt");
////MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag");
////MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag");
////MultipleTypeTree.InitTreeModel("../../models/apricot_tree/tree.obj", 0);
//
//	CInitMultipleTypeTree MultipleTypeTree(Common::TreesTypeNumber, ALLTreeNumber, Common::GenerateRandomTreePosition);
//	//////////////////////////////////////////
//	//MultipleTypeTree.InitShadowCubeMapPara(near_plane, far_plane, SHADOW_WIDTH, SHADOW_HEIGHT, shadowTransforms, lightVertices, lightColors);
//	const int TreeRelatedFileNumber[Common::TreesTypeNumber] = { 9,9 };
//	for (int i = 0; i < Common::TreesTypeNumber; i++)
//	{
//		std::string MotiondataPath = "F:/GraduationProject/exper4/motiondata";
//		MotiondataPath += std::to_string(i);
//		MultipleTypeTree.InitVegaFemFactory(MotiondataPath, "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree_last_test.obj", "../../models/yellow_tree/ObjectVertexIndex.txt", TreeRelatedFileNumber[i]);
//		MultipleTypeTree.InitWindAndTree(Common::TreesNumbers[0], "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/WindAndTreeConfig/Config.txt");
//		if (Common::UseGeomOrCompCalculateNormal == true)
//		{
//			MultipleTypeTree.InitSceneShadowShader("scene_shadows.vert", "scene_shadows.frag", "scene_shadows.geom");
//		}
//		else
//		{
//			MultipleTypeTree.InitSceneShadowShader("sence_shadows_RelatedComp.vert", "sence_Shadows_RelatedComp.frag");
//			MultipleTypeTree.InitComputerShaderCalculateNormal("calculateNormal.comp");
//		}
//		//MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag","computerFaceNormal.geom");
//		MultipleTypeTree.InitSceneDepthShader("point_shadows_depth.vert", "point_shadows_depth.frag");
//		MultipleTypeTree.InitTreeModel("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree_last_test.obj", 0);
//	}
//
//
//	for (int i = 0; i < Common::TreesTypeNumber; i++)
//	{
//		MultipleTypeTree.InitMultipleExtraWindData(i);
//		MultipleTypeTree.InitFemFrameStruct(i);
//		MultipleTypeTree.InitScenceShaderData(i, Common::ScaleTree[i]);
//
//
//		//End Each time change*************
//		//***************�˴�Ҳ�޸���InstanceMatrix����תVAO
//		if (Common::renderingTerrainOrWhiltScence == true)
//		{
//			MultipleTypeTree.calculateTreeDistantWithTerrain(i);
//			MultipleTypeTree.updataTreeOnTerrain(i);
//		}
//
//		//***************
//
//	}
//	MultipleTypeTree.setTreeAxisYPara();
//
//	for (int i = 0; i < Common::TreesTypeNumber; i++)
//	{
//		MultipleTypeTree.updataTreeOnTerrainByPara(i);
//	}
//
//	for (int i = 0; i < Common::TreesTypeNumber; i++)
//	{
//		EachFormNumberArray.push_back(MultipleTypeTree.getSpecificLoadWindAndTree(i).getEachFormNumberArray());
//
//		//��Ϊ��updataTreeOnTerrain�޸���InstanceMatrix����д�����
//		if (Common::UseGeomOrCompCalculateNormal == false)
//		{
//			MultipleTypeTree.InitScenceNormalMatrixData(i);
//		}
//	}
//#pragma endregion
//
//
//	//InsertSearchTreeFrameIndex(*(MultipleTypeTree.getSpecificFemFactory(0)), *(MultipleTypeTree.getSpecificTreeModel(0)), *(MultipleTypeTree.getSpecificExtraForces(0)), *(MultipleTypeTree.getSpecificExtraDirection(0)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(0)), 0);
//	//�����߳̽��ж�ȡTree����
//		/////each time change
//	//boost::thread startInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getFemFactory()), *(MultipleTypeTree.getTreeModel()), *(MultipleTypeTree.getExtraForces()), *(MultipleTypeTree.getExtraDirection()), *(MultipleTypeTree.getTreesNumberSubjected2SameWind()),MultipleTypeTree.getTreeTypeIndex());
//	boost::thread startInsertIntoQueue0 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(0)), *(MultipleTypeTree.getSpecificTreeModel(0)), *(MultipleTypeTree.getSpecificExtraForces(0)), *(MultipleTypeTree.getSpecificExtraDirection(0)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(0)), 0);
//	//boost::thread startInsertIntoQueue1 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(1)), *(MultipleTypeTree.getSpecificTreeModel(1)), *(MultipleTypeTree.getSpecificExtraForces(1)), *(MultipleTypeTree.getSpecificExtraDirection(1)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(1)), 1);
//	/*boost::thread startInsertIntoQueue2 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(2)), *(MultipleTypeTree.getSpecificTreeModel(2)), *(MultipleTypeTree.getSpecificExtraForces(2)), *(MultipleTypeTree.getSpecificExtraDirection(2)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(2)), 2);
//	boost::thread startInsertIntoQueue3 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(3)), *(MultipleTypeTree.getSpecificTreeModel(3)), *(MultipleTypeTree.getSpecificExtraForces(3)), *(MultipleTypeTree.getSpecificExtraDirection(3)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(3)), 3);
//	boost::thread startInsertIntoQueue4 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(4)), *(MultipleTypeTree.getSpecificTreeModel(4)), *(MultipleTypeTree.getSpecificExtraForces(4)), *(MultipleTypeTree.getSpecificExtraDirection(4)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(4)), 4);
//	boost::thread startInsertIntoQueue5 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(5)), *(MultipleTypeTree.getSpecificTreeModel(5)), *(MultipleTypeTree.getSpecificExtraForces(5)), *(MultipleTypeTree.getSpecificExtraDirection(5)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(5)), 5);
//	boost::thread startInsertIntoQueue6 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(6)), *(MultipleTypeTree.getSpecificTreeModel(6)), *(MultipleTypeTree.getSpecificExtraForces(6)), *(MultipleTypeTree.getSpecificExtraDirection(6)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(6)), 6);
//	boost::thread startInsertIntoQueue7 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(7)), *(MultipleTypeTree.getSpecificTreeModel(7)), *(MultipleTypeTree.getSpecificExtraForces(7)), *(MultipleTypeTree.getSpecificExtraDirection(7)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(7)), 7);
//	boost::thread startInsertIntoQueue8 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(8)), *(MultipleTypeTree.getSpecificTreeModel(8)), *(MultipleTypeTree.getSpecificExtraForces(8)), *(MultipleTypeTree.getSpecificExtraDirection(8)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(8)), 8);
//	boost::thread startInsertIntoQueue9 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(9)), *(MultipleTypeTree.getSpecificTreeModel(9)), *(MultipleTypeTree.getSpecificExtraForces(9)), *(MultipleTypeTree.getSpecificExtraDirection(9)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(9)), 9);
//	boost::thread startInsertIntoQueue10 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(10)), *(MultipleTypeTree.getSpecificTreeModel(10)), *(MultipleTypeTree.getSpecificExtraForces(10)), *(MultipleTypeTree.getSpecificExtraDirection(10)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(10)), 10);
//	boost::thread startInsertIntoQueue11 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(11)), *(MultipleTypeTree.getSpecificTreeModel(11)), *(MultipleTypeTree.getSpecificExtraForces(11)), *(MultipleTypeTree.getSpecificExtraDirection(11)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(11)), 11);
//	boost::thread startInsertIntoQueue12 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(12)), *(MultipleTypeTree.getSpecificTreeModel(12)), *(MultipleTypeTree.getSpecificExtraForces(12)), *(MultipleTypeTree.getSpecificExtraDirection(12)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(12)), 12);
//	boost::thread startInsertIntoQueue13 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(13)), *(MultipleTypeTree.getSpecificTreeModel(13)), *(MultipleTypeTree.getSpecificExtraForces(13)), *(MultipleTypeTree.getSpecificExtraDirection(13)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(13)), 13);
//	boost::thread startInsertIntoQueue14 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(14)), *(MultipleTypeTree.getSpecificTreeModel(14)), *(MultipleTypeTree.getSpecificExtraForces(14)), *(MultipleTypeTree.getSpecificExtraDirection(14)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(14)), 14);*/
//	//boost::thread startInsertIntoQueue15 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(15)), *(MultipleTypeTree.getSpecificTreeModel(15)), *(MultipleTypeTree.getSpecificExtraForces(15)), *(MultipleTypeTree.getSpecificExtraDirection(15)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(15)), 15);
//	//boost::thread startInsertIntoQueue16 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(16)), *(MultipleTypeTree.getSpecificTreeModel(16)), *(MultipleTypeTree.getSpecificExtraForces(16)), *(MultipleTypeTree.getSpecificExtraDirection(16)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(16)), 16);
//	//boost::thread startInsertIntoQueue17 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(17)), *(MultipleTypeTree.getSpecificTreeModel(17)), *(MultipleTypeTree.getSpecificExtraForces(17)), *(MultipleTypeTree.getSpecificExtraDirection(17)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(17)), 17);
//	//boost::thread startInsertIntoQueue18 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(18)), *(MultipleTypeTree.getSpecificTreeModel(18)), *(MultipleTypeTree.getSpecificExtraForces(18)), *(MultipleTypeTree.getSpecificExtraDirection(18)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(18)), 18);
//	//boost::thread startInsertIntoQueue19 = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(19)), *(MultipleTypeTree.getSpecificTreeModel(19)), *(MultipleTypeTree.getSpecificExtraForces(19)), *(MultipleTypeTree.getSpecificExtraDirection(19)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(19)), 19);
//	
//	//boost::thread SecondstartInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(1)), *(MultipleTypeTree.getSpecificTreeModel(1)), *(MultipleTypeTree.getSpecificExtraForces(1)), *(MultipleTypeTree.getSpecificExtraDirection(1)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(1)), 1);
//	//boost::thread ThirdstartInsertIntoQueue = boost::thread(InsertSearchTreeFrameIndex, *(MultipleTypeTree.getSpecificFemFactory(2)), *(MultipleTypeTree.getSpecificTreeModel(2)), *(MultipleTypeTree.getSpecificExtraForces(2)), *(MultipleTypeTree.getSpecificExtraDirection(2)), *(MultipleTypeTree.getSpecificTreesNumberSubjected2SameWind(2)), 2);
//
//	MultipleTypeTree.ComputeSumFaceVerticesBeforeEndMesh(Common::TreesTypeNumber);
//	glm::mat4 model = glm::mat4(1.0f);
//	glm::mat4 projection;
//	glm::mat4 view;
//
//#pragma region load paper experiment
//	//SET VegMesh
//	if (Common::renderingSurfaceMesh == true)
//	{
//		RenderStaticSence.loadStaticModel("TreeMeshVeg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/18/tree18.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("TreeMeshVeg", *MultipleTypeTree.GetFirstTreeModelMatrix());
//		RenderStaticSence.initModelShaderVegPara("TreeMeshVeg", *MultipleTypeTree.GetFirstTreeModelMatrix());
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("TreeMeshVeg", glm::vec4(0, 0.9, 0.8, 0));
//	}
//
//	if (Common::renderingStemMesh == true)
//	{
//		RenderStaticSence.loadStaticModel("StemVeg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/50/steam/steam50.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("StemVeg", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("StemVeg");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("StemVeg", glm::vec4(0.73, 0, 0, 0));
//
//		RenderStaticSence.loadStaticModel("StemObject", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/stem.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("StemObject", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("StemObject");
//
//	}
//
//	if (Common::renderingLeafMesh == true)
//	{
//		RenderStaticSence.loadStaticModel("LeafVeg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/90/leaf/leaf90.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("LeafVeg", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("LeafVeg");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("LeafVeg", glm::vec4(0.278, 0.73, 0, 0));
//
//		RenderStaticSence.loadStaticModel("LeafObject", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/leaf.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("LeafObject", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("LeafObject");
//
//	}
//
//	if (Common::renderingFibrous == true)
//	{
//		RenderStaticSence.loadStaticModel("FibrousVeg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/90/fibrous/fibrous90.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("FibrousVeg", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("FibrousVeg");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("FibrousVeg", glm::vec4(0, 0.474, 1, 0));
//
//		RenderStaticSence.loadStaticModel("FibrousObject", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/fibrous_all.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("FibrousObject", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("FibrousObject");
//
//	}
//
//	//testRenderVegLine
//	if (Common::renderAllVeg == true)
//	{
//		RenderStaticSence.loadStaticModel("StemVegLine", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/50/steam/steam50.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("StemVegLine", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("StemVegLine");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("StemVegLine", glm::vec4(0, 0, 0, 0));
//
//		RenderStaticSence.loadStaticModel("LeafVegLine", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/90/leaf/leaf90.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("LeafVegLine", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("LeafVegLine");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("LeafVegLine", glm::vec4(0, 0, 0, 0));
//
//		RenderStaticSence.loadStaticModel("FibrousVegLine", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/SplitDifferentPart/90/fibrous/fibrous90.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("VegSurfaceWireFrame.vert", "VegSurfaceWireFrame.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("FibrousVegLine", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("FibrousVegLine");
//		RenderStaticSence.setVegMeshFragmentShaderRenderingColor("FibrousVegLine", glm::vec4(0, 0, 0, 0));
//	}
//
//	// Rendering experiment ModelMotion
//	if (Common::renderingSameModelMotion == true)
//	{
//		//RenderStaticSence.loadStaticModel("ModelPosture1", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/ModelMotion/5.obj");
//		RenderStaticSence.loadStaticModel("ModelPosture1", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree_last_test.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("ModelPosture1", glm::vec3(0, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("ModelPosture1");
//
//		/*RenderStaticSence.loadStaticModel("ModelPosture2", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/ModelMotion/10.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("ModelPosture2", glm::vec3(4, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("ModelPosture2");
//
//		RenderStaticSence.loadStaticModel("ModelPosture3", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/ModelMotion/15.obj");
//		RenderStaticSence.loadDepthShader("grass_shadows_depth.vert", "point_shadows_depth.frag");
//		RenderStaticSence.loadSenceShader("grass.vert", "grass.frag");
//		RenderStaticSence.setModelScale(0.2f);
//		RenderStaticSence.setObjectTransform("ModelPosture3", glm::vec3(8, 0, 0), 0);
//		RenderStaticSence.initModelShaderPara("ModelPosture3");*/
//	}
//
//	CubicVegMesh* VegMesh = nullptr;
//	CShader * CubicVegShader = nullptr;
//	CShader * CubicFiexedVegShader = nullptr;
//	if (Common::renderingVegMesh == true)
//	{
//
//		//VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/100/EachPartSkelCubic_index/tree100Rendering.veg","D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/EachPartSkelCubic_index/treebase.txt",false,true);
//		//VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/100/ObjMappingVoxel/tree100RenderingObjMapping.veg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OtherVegType/100/ObjMappingVoxel/treebase.txt", false, true);
//		VegMesh = new CubicVegMesh("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber110/tree70.veg", "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber110/EachPartSkelCubic_index/treebase.txt", false, true);
//		VegMesh->InitVegRenderingProcess();
//		VegMesh->InitVegRenderingLabeledVoxel();
//		CubicVegShader = new CShader("VegMesh.vert", "VegMesh.frag");
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
//		CubicVegShader->use();
//		CubicVegShader->setMat4("model", model);
//
//		CubicFiexedVegShader = new CShader("VegMesh.vert", "VegMesh.frag");
//		CubicFiexedVegShader->use();
//		CubicFiexedVegShader->setMat4("model", model);
//
//	}
//
//#pragma endregion
//	////////////////////////
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//
//	// Setup Dear ImGui style
//	ImGui::StyleColorsDark();
//	ImGui::StyleColorsClassic();
//
//	// Setup Platform/Renderer backends
//	ImGui_ImplGlfw_InitForOpenGL(Window, true);
//	ImGui_ImplOpenGL3_Init(glsl_version);
//
//
//	// Our state
//	bool show_demo_window = true;
//	bool show_another_window = false;
//	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//	std::vector<float> tempFrame;
//	std::vector<Common::SConnectedFemFiles> vAllReallyLoadConnectedFem = (MultipleTypeTree.getSpecificFemFactory(0))->getAllReallyLoadConnectedFem();
//
//	/*std::ofstream outputFilePath;
//	outputFilePath.open("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber440/EachPartSkelCubic_index_hausdorffobj/19.txt", std::ios::in | std::ios::app);
//	std::ofstream outputFilePath1;
//	outputFilePath1.open("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber440/EachPartSkelCubic_index_hausdorffobj/99.txt", std::ios::in | std::ios::app);
//	std::ofstream outputFilePath2;
//	outputFilePath2.open("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/OthrVegType/70/Hausdorff433/voxelnumber440/EachPartSkelCubic_index_hausdorffobj/179.txt", std::ios::in | std::ios::app);*/
//	int numFramePoints = vAllReallyLoadConnectedFem[0].FemDataset[0]->Frames[0].BaseFileDeformations.size();
//	std::vector<glm::vec3>sumDeltaU(numFramePoints);
//
//	while (!glfwWindowShouldClose(Window))
//	{
//#pragma region Imgui 
//		// Start the Dear ImGui frame
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//
//		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//		//if (show_demo_window)
//			//ImGui::ShowDemoWindow(&show_demo_window);
//
//		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//		{
//			static float f = 0.0f;
//			static int counter = 0;
//
//			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
//			float ImguiWeight = 350;
//			float ImguiHight = 250;
//			ImGui::SetWindowSize(ImVec2(ImguiWeight, ImguiHight));
//			ImGui::Text("FrameIndex:%d", frameIndex);               // Display some text (you can use a format strings too)
//			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//			ImGui::Checkbox("Another Window", &show_another_window);
//
//			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//				counter++;
//			ImGui::SameLine();
//			ImGui::Text("counter = %d", counter);
//			ImGui::Text("CameraPosition: (%f, %f, %f)", Camera.getPositionX(), Camera.getPositionY(), Camera.getPositionZ());
//			ImGui::Text("CamerarFront: (%f, %f, %f)", Camera.getFrontX(), Camera.getFrontY(), Camera.getFrontZ());
//			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//			ImGui::End();
//		}
//		//std::cout << ImGui::GetIO().Framerate << std::endl;
//		tempFrame.push_back(ImGui::GetIO().Framerate);
//		// 3. Show another simple window.
//		if (show_another_window)
//		{
//			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//			ImGui::Text("Hello from another window!");
//			if (ImGui::Button("Close Me"))
//				show_another_window = false;
//			ImGui::End();
//		}
//
//		////// Rendering
//		ImGui::Render();
//		int display_w, display_h;
//		glfwGetFramebufferSize(Window, &display_w, &display_h);
//		glViewport(0, 0, display_w, display_h);
//		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//		glClear(GL_COLOR_BUFFER_BIT);
//#pragma endregion
//		if (automoveCamera == true)
//		{
//			Camera.UpdataCameraPosition(FrameNumber);
//		}
//
//
//
//		//////////////////
//		// per-frame time logic
//		float currentFrame = glfwGetTime();
//		DeltaTime = (currentFrame - LastFrame);
//		LastFrame = currentFrame;
//
//		// input---
//
//
//		// render----
//		glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		//ÿ�����������ν���һ����Ⱦʱ��֡D_sum reset
//		/*if (FrameNumber%Common::ProductFrameNumber == 0)*/
//		/*if (FrameNumber%180 == 0)
//		{
//			ourModel.resetSSBO4UDeformation();
//
//			std::cout << "//////////////////////////////////////" << std::endl;
//			std::cout << "Reset" << std::endl;
//		}*/
//		std::vector<std::pair<int, int>> tempTreeFileAndFrameIndex;
//
//		for (int i = 0; i < Common::TreesTypeNumber; i++)
//		{
//			bool Success = SearchQueue[i].TryDequeue(tempTreeFileAndFrameIndex);
//
//			//std::cout << "[";
//   //         for (int k = 0; k < EachFormNumberArray[i].size(); k++)
//   //         {
//   //             std::cout << tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].first << "--" << tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].second << "||";
//
//
//   ////             //std::string headInfo = "Position" + std::to_string(frameIndex);
//   ////             //outputFilePath << headInfo << "\n";
//   ////             //outputFilePath << numFramePoints << "\n";
//   //             for (int j = 0; j < numFramePoints; j++)
//   //             {
//   //                 sumDeltaU[j].x += vAllReallyLoadConnectedFem[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].first].FemDataset[0]->Frames[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].second].BaseFileDeformations[j].x;
//   //                 sumDeltaU[j].y += vAllReallyLoadConnectedFem[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].first].FemDataset[0]->Frames[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].second].BaseFileDeformations[j].y;
//   //                 sumDeltaU[j].z += vAllReallyLoadConnectedFem[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].first].FemDataset[0]->Frames[tempTreeFileAndFrameIndex[EachFormNumberArray[i][k] - 1].second].BaseFileDeformations[j].z;
//   //                 if (frameIndex == 19)
//   //                 {
//   //                     outputFilePath << sumDeltaU[j].x << " ";
//   //                     outputFilePath << sumDeltaU[j].y << " ";
//   //                     outputFilePath << sumDeltaU[j].z << "\n";
//   //                 }
//   //                 else if (frameIndex == 99)
//   //                 {
//   //                     outputFilePath1 << sumDeltaU[j].x << " ";
//   //                     outputFilePath1 << sumDeltaU[j].y << " ";
//   //                     outputFilePath1 << sumDeltaU[j].z << "\n";
//   //                 }
//   //                 else if (frameIndex == 179)
//   //                 {
//   //                     outputFilePath2 << sumDeltaU[j].x << " ";
//   //                     outputFilePath2 << sumDeltaU[j].y << " ";
//   //                     outputFilePath2 << sumDeltaU[j].z << "\n";
//   //                 }
//   //             }
//   //         }
//   //         std::cout << "]";
//
//
//
//			MultipleTypeTree.getSpecificTreeModel(i)->UpdataSSBOMeshTreeAndFrameIndex(tempTreeFileAndFrameIndex);
//			tempTreeFileAndFrameIndex.clear();
//		}
//
//		//std::cout << std::endl;
//
//		FrameNumber++;
//
//		// 1. render DepthMap
//		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//
//		//plane
//		RenderStaticSence.setSencePara(Camera, SCR_WIDTH, SCR_HEIGHT);
//		MultipleTypeTree.UpdataSencePara(Camera, SCR_WIDTH, SCR_HEIGHT);
//		//grass
//
//		RenderStaticSence.RenderingDepth("terrain");
//		RenderStaticSence.RenderingDepth("greenGrass");
//
//		//tree
//		for (int i = 0; i < Common::TreesTypeNumber; i++)
//		{
//			MultipleTypeTree.RenderingDepth(i, 1, glfwGetTime(), frameIndex, 9);
//			renderTree(*(MultipleTypeTree.getSpecificScenceDepthShader(i)), *(MultipleTypeTree.getSpecificTreeModel(i)));
//			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//		}
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		//computerNormal
//
//		if (Common::UseGeomOrCompCalculateNormal == false)
//		{
//
//			for (int i = 0; i < Common::TreesTypeNumber; i++)
//			{
//				//MultipleTypeTree.getSpecificTreeModel(i)->ComputerShaderCalculateNormal(*Ourcomputershader);
//				MultipleTypeTree.ComputeNormal(i);
//				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//			}
//		}
//
//		//2.render Scene
//		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		////plane  
//		/////////////grass
//
//
//		RenderStaticSence.RenderingModel("terrain", depthMap, shadows);
//		if (Common::renderingGrass == true)
//			RenderStaticSence.RenderingModel("greenGrass", depthMap, shadows);
//		if (Common::renderingLightSource == true)
//			RenderStaticSence.RenderingModel("lightsource", depthMap, false);
//
//		glm::mat4 temp = *MultipleTypeTree.GetFirstTreeModelMatrix();
//
//		//tree
//		if (Common::renderingForest == true)
//		{
//			for (int i = 0; i < Common::TreesTypeNumber; i++)
//			{
//				MultipleTypeTree.RenderingModel(i, depthMap, 1, glfwGetTime(), frameIndex, 9, bendScale[i], primaryOffsetScale[i], shadows);
//
//				renderTree(*(MultipleTypeTree.getSpecificScenceShadowShader(i)), *(MultipleTypeTree.getSpecificTreeModel(i)));
//			}
//		}
//
//		//if (i < 999)
//		//{
//		//	std::string pathString = "video5/" + std::to_string(i) + ".jpg";
//		//	captureScreen2Img(pathString, 100);
//		//}
//		//Camera.outFront();
//		//i++;
//		//skybox	
//
//		//std::cout << i << std::endl;
//		ourSkyBoxShader.use();
//		renderSkybox(ourSkyBoxShader, skyboxVAO, cubemapTexture);
//
//
//		//Sleep(50);
//		frameIndex++;
//		glDepthFunc(GL_LESS); // set depth function back to default
//
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//		processInput(Window);
//		glfwSwapBuffers(Window);
//		glfwPollEvents();
//
//	}
//
//
//	/* outputFilePath.close();
//	 outputFilePath1.close();
//	 outputFilePath2.close();*/
//	 /*int sum = 0;
//	 for (int i = 0; i < tempFrame.size(); i++)
//	 {
//		 sum += tempFrame[i];
//	 }
//	 std::cout << sum / frameIndex << std::endl;*/
//
//	glDeleteVertexArrays(1, &skyboxVAO);
//	glDeleteVertexArrays(1, &planeVAO);
//	glDeleteBuffers(1, &planeVBO);
//	glDeleteBuffers(1, &skyboxVBO);
//	//glDeleteBuffers(1, &SSBO);
//	MultipleTypeTree.getSpecificTreeModel(0)->Clear();
//	// Cleanup
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(Window);
//
//
//	glfwTerminate();
//	//_CrtDumpMemoryLeaks();
//	return 0;
//}
//
////*********************************************************************
////FUNCTION:
//void renderPlane(CShader& vShader, const unsigned int& VAOId, const unsigned int& vTextureId, const unsigned int& vTextureOpacityId)
//{
//	glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	glm::mat4 view = Camera.getViewMatrix();
//	glm::mat4 model = glm::mat4(1.0f);
//	vShader.setMat4("projection", projection);
//	vShader.setMat4("view", view);
//	vShader.setInt("texture_diffuse1", 0);
//	vShader.setInt("texture_opacity1", 1);
//	glBindVertexArray(VAOId);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, vTextureId);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, vTextureOpacityId);
//	vShader.setMat4("model", model);
//	//vShader.setMat4("model1", model);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//
////*********************************************************************
////FUNCTION:
//void renderTree(CShader & vShader, CSence& vModel)
//{
//	glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	glm::mat4 view = Camera.getViewMatrix();
//	vShader.setMat4("projection", projection);
//	vShader.setMat4("view", view);
//	vShader.setVec3("camPos", Camera.getPosition());
//
//	//glm::mat4 model = glm::mat4(1.0f);
//	////model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));// translate it down so it's at the center of the scene
//	////model = glm::translate(model, glm::vec3(0.0f, -0.3f, -2.0f));
//	//model = glm::translate(model, glm::vec3(0.0f, -0.25f, -1.2f));
//	//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
//	//vShader.setMat4("model", model);
//
//
//	vModel.draw(vShader, Common::DrawType::TRIANGLES);
//
//}
//
////*********************************************************************
////FUNCTION:
//void renderLight(CShader & vShader, const unsigned int & VAOId)
//{
//	glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	glm::mat4 view = Camera.getViewMatrix();
//	glm::mat4 model = glm::mat4(1.0f);
//	vShader.setMat4("projection", projection);
//	vShader.setMat4("view", view);
//	vShader.setMat4("model", model);
//	glBindVertexArray(VAOId);
//	glDrawArrays(GL_LINES, 0, 3);
//}
//
////*********************************************************************
////FUNCTION:
//void renderSkybox(CShader & vShader, const unsigned int & VAOId, const unsigned int & vTextureId)
//{
//	glDepthFunc(GL_LEQUAL);
//	//view = Camera.getViewMatrix();//stop move skybox
//	glm::mat4  view = glm::mat4(glm::mat3(Camera.getViewMatrix()));
//	glm::mat4 projection = glm::perspective(glm::radians(Camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//	vShader.setMat4("view", view);
//	vShader.setMat4("projection", projection);
//	glBindVertexArray(VAOId);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, vTextureId);
//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	glBindVertexArray(0);
//}
//
////*********************************************************************
////FUNCTION:
//bool initWindow(GLFWwindow*& vWindow, int vScreenWidth, int vScreenHeight)
//{
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
//#endif
//
//	vWindow = glfwCreateWindow(vScreenWidth, vScreenHeight, "LargeScaleForest", NULL, NULL);
//	if (vWindow == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return false;
//	}
//	glfwMakeContextCurrent(vWindow);
//	glfwSetFramebufferSizeCallback(vWindow, framebufferSizeCallback);
//	glfwSetCursorPosCallback(vWindow, mouseCallback);
//	glfwSetScrollCallback(vWindow, scrollCallback);
//	glfwSetInputMode(vWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//
//	glewExperimental = true;
//	if (glewInit() != GLEW_OK)
//	{
//		std::cout << "glew init failed." << std::endl;
//		glfwTerminate();
//		return false;
//	}
//	return true;
//}
//
////*********************************************************************
////FUNCTION:
//int captureScreen2Img(const std::string& vFileName, int vQuality)
//{
//	size_t Start = vFileName.find_last_of('.');
//
//	std::string FilePostfix = vFileName.substr(Start + 1, vFileName.size() - Start);
//	int WindowWidth = 0, WindowHeight = 0;
//	WindowWidth = SCR_WIDTH;
//	WindowHeight = SCR_HEIGHT;
//	std::shared_ptr<GLbyte> pScreenData;
//	pScreenData.reset(new GLbyte[WindowWidth * WindowHeight * 4]);
//	glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, pScreenData.get());
//	stbi_flip_vertically_on_write(true);
//
//	if (FilePostfix == "tga")
//		return stbi_write_tga(vFileName.c_str(), WindowWidth, WindowHeight, 4, pScreenData.get());
//	else if (FilePostfix == "png")
//		return stbi_write_png(vFileName.c_str(), WindowWidth, WindowHeight, 4, pScreenData.get(), WindowWidth * 4);
//	else if (FilePostfix == "bmp")
//		return stbi_write_bmp(vFileName.c_str(), WindowWidth, WindowHeight, 4, pScreenData.get());
//	else if (FilePostfix == "jpg")
//		return stbi_write_jpg(vFileName.c_str(), WindowWidth, WindowHeight, 4, pScreenData.get(), vQuality);
//	return -1;
//}
//
////*********************************************************************
////FUNCTION:
//void processInput(GLFWwindow* vWindow)
//{
//	std::string pathString = std::to_string(i) + ".png";
//	if (glfwGetKey(vWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(vWindow, true);
//
//	float cameraSpeed = 2.5f * DeltaTime; // adjust accordingly
//	if (glfwGetKey(vWindow, GLFW_KEY_W) == GLFW_PRESS)
//		Camera.processKeyboard(FORWARD, DeltaTime);
//	if (glfwGetKey(vWindow, GLFW_KEY_S) == GLFW_PRESS)
//		Camera.processKeyboard(BACKWARD, DeltaTime);
//	if (glfwGetKey(vWindow, GLFW_KEY_A) == GLFW_PRESS)
//		Camera.processKeyboard(LEFT, DeltaTime);
//	if (glfwGetKey(vWindow, GLFW_KEY_D) == GLFW_PRESS)
//		Camera.processKeyboard(RIGHT, DeltaTime);
//	if (glfwGetKey(vWindow, GLFW_KEY_Q) == GLFW_PRESS)
//	{
//		captureScreen2Img(PathString, 100);
//		std::cout << "caputure Image in PathString" << std::endl;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_T) == GLFW_PRESS)
//	{
//		Camera.setstartsaveData(startsaveCameraPara);
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_Y) == GLFW_PRESS)
//	{
//		Camera.outputfile("G:\\GraduationProject\\Camera\\position.txt");
//	}
//
//	/*if (glfwGetKey(vWindow, GLFW_KEY_J) == GLFW_PRESS)
//	{
//		Camera.processMouseMovement(float vXOffset, float vYOffset, GLboolean vConstrainPitch)
//	}*/
//	if (glfwGetKey(vWindow, GLFW_KEY_K) == GLFW_PRESS)
//	{
//		if (MouseShow == true)
//		{
//			glfwSetInputMode(vWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//			MouseShow = false;
//		}
//		else
//		{
//			glfwSetInputMode(vWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//			MouseShow = true;
//		}
//
//	}
//
//
//	if (glfwGetKey(vWindow, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
//	{
//		shadows = !shadows;
//		shadowsKeyPressed = true;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_SPACE) == GLFW_RELEASE)
//	{
//		shadowsKeyPressed = false;
//	}
//
//	if (glfwGetKey(vWindow, GLFW_KEY_1) == GLFW_PRESS)
//	{
//		VoxelGroupIndex = 0;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_2) == GLFW_PRESS)
//	{
//		VoxelGroupIndex = 1;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_3) == GLFW_PRESS)
//	{
//		VoxelGroupIndex = 2;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_4) == GLFW_PRESS)
//	{
//		VoxelGroupIndex = 3;
//	}
//	if (glfwGetKey(vWindow, GLFW_KEY_0) == GLFW_PRESS)
//	{
//		VoxelGroupIndex = 11;
//	}
//}
//
////*********************************************************************
////FUNCTION:
//void mouseCallback(GLFWwindow* vWindow, double vXPos, double vYPos)
//{
//	if (FirstMouse)
//	{
//		LastX = vXPos;
//		LastY = vYPos;
//		FirstMouse = false;
//	}
//
//	float XOffset = vXPos - LastX;
//	float YOffset = LastY - vYPos;
//	LastX = vXPos;
//	LastY = vYPos;
//
//	//if (XOffset >0)
//	//{
//	//	XOffset = 20;
//	//}
//	//else
//	//{
//	//	XOffset = -20;
//	//}
//
//	//if (YOffset>0)
//	//{
//	//	YOffset = 20;
//	//}
//	//else
//	//{
//	//	YOffset = -20;
//	//}
//
//	Camera.processMouseMovement(XOffset, YOffset);
//}
//
////*********************************************************************
////FUNCTION:
//void scrollCallback(GLFWwindow* vWindow, double vXOffset, double vYOffset)
//{
//	Camera.processMouseScroll(vYOffset);
//}
//
////*********************************************************************
////FUNCTION:
//void framebufferSizeCallback(GLFWwindow* vWindow, int vWidth, int vHeight)
//{
//	glViewport(0, 0, vWidth, vHeight);
//}
//
//// utility function for loading a 2D texture from file
//unsigned int loadTexture(char const * path)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}
//
////*********************************************************************
////FUNCTION:
//unsigned int loadCubemap(std::vector<std::string> faces)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	int width, height, nrChannels;
//	for (unsigned int i = 0; i < faces.size(); i++)
//	{
//		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//		if (data)
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			stbi_image_free(data);
//		}
//		else
//		{
//			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//			stbi_image_free(data);
//		}
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}
//
//