/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "Interactive deformable object simulator" driver application,         *
 *  Copyright (C) 2007 CMU, 2009 MIT, 2018 USC                           *
 *                                                                       *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code authors: Jernej Barbic, Fun Shing Sin, Daniel Schroeder          *
 * http://www.jernejbarbic.com/vega                                      *
 *                                                                       *
 * Research: Jernej Barbic, Hongyi Xu, Yijing Li,                        *
 *           Danyong Zhao, Bohan Wang,                                   *
 *           Fun Shing Sin, Daniel Schroeder,                            *
 *           Doug L. James, Jovan Popovic                                *
 *                                                                       *
 * Funding: National Science Foundation, Link Foundation,                *
 *          Singapore-MIT GAMBIT Game Lab,                               *
 *          Zumberge Research and Innovation Fund at USC,                *
 *          Sloan Foundation, Okawa Foundation,                          *
 *          USC Annenberg Foundation                                     *
 *                                                                       *
 * This utility is free software; you can redistribute it and/or         *
 * modify it under the terms of the BSD-style license that is            *
 * included with this library in the file LICENSE.txt                    *
 *                                                                       *
 * This utility is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the file     *
 * LICENSE.TXT for more details.                                         *
 *                                                                       *
 *************************************************************************/

/*****************************************************************************

Interactive deformable object simulator (3D nonlinear solid deformable objects).

Supported integrators:
- implicit backward Euler
- implicit Newmark
- central differences
- Euler
- symplectic Euler

Supported meshes:
- tetrahedral
- cubic

Supported materials:
- linear
- rotated linear (corotational linear FEM)
- Saint-Venant Kirchoff
- invertible Saint-Venant Kirchoff
- invertible neo-Hookean 
- invertible Mooney-Rivlin
- mass-spring system

*******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cassert>
#include <float.h>
using namespace std;

#if defined(WIN32) || defined(_WIN32)
  #include <windows.h>
#endif

#ifdef __APPLE__
  #include "TargetConditionals.h"
#endif


#include "Shader.h"
#include "Sence.h"
#include "Mesh.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "getopts.h"
#include "initGraphics.h"
#include "sceneObjectDeformable.h"
#include "performanceCounter.h"

#include "volumetricMeshLoader.h"
#include "tetMesh.h"

#include "StVKElementABCDLoader.h"
#include "StVKCubeABCD.h"
#include "StVKTetABCD.h"
#include "StVKTetHighMemoryABCD.h"
#include "StVKFEM.h"
#include "StVKStencilForceModel.h"

#include "implicitBackwardEulerSparse.h"
#include "eulerSparse.h"
#include "centralDifferencesSparse.h"

#include "corotationalLinearFEM.h"
#include "corotationalLinearFEMStencilForceModel.h"

#include "isotropicHyperelasticFEM.h"
#include "isotropicMaterial.h"
#include "StVKIsotropicMaterial.h"
#include "neoHookeanIsotropicMaterial.h"
#include "MooneyRivlinIsotropicMaterial.h"
#include "isotropicHyperelasticFEMStencilForceModel.h"

#include "getIntegratorSolver.h"
#include "generateMeshGraph.h"
#include "generateMassMatrix.h"

#include "massSpringSystem.h"
#include "massSpringSystemFromObjMeshConfigFile.h"
#include "massSpringSystemFromTetMeshConfigFile.h"
#include "massSpringSystemFromCubicMeshConfigFile.h"
#include "massSpringStencilForceModel.h"

#include "linearFEMStencilForceModel.h"

#include "forceModelAssembler.h"

#include "graph.h"
#include "renderSprings.h"
#include "configFile.h"

#include "lighting.h"
#include "listIO.h"
#include "matrixIO.h"
#include "averagingBuffer.h"

#include "GL/glui.h"



// graphics 
char windowTitleBase[4096] = "Real-time sim";
void displayFunction(void);
int windowID;
int windowWidth = 800;
int windowHeight = 600;
double zNear=0.01;
double zFar=10.0;
double cameraRadius;
double focusPositionX, focusPositionY, focusPositionZ;
double cameraLongitude, cameraLattitude;
SphericalCamera * camera = nullptr;
int g_iLeftMouseButton=0, g_iMiddleMouseButton=0, g_iRightMouseButton=0;
int g_vMousePos[2] = {0,0};
int shiftPressed=0;
int altPressed=0;
int ctrlPressed=0;
int renderWireframe=1;
int renderAxes=0;
int renderDeformableObject=1;
int renderSecondaryDeformableObject=1;
int useRealTimeNormals = 0;
int renderGroundPlane = 0;
int renderFixedVertices = 1;
int renderSprings = 0;
int renderVertices = 0;
int lockScene=0;
int pauseSimulation=0;
int singleStepMode=0;
Lighting * lighting = nullptr;
//加载包围网格
SceneObjectDeformable * deformableObjectRenderingMesh = nullptr;
//源文件
SceneObjectDeformable * secondaryDeformableObjectRenderingMesh = nullptr;
SceneObject * extraSceneGeometry = nullptr;
char groundPlaneString[128];
double groundPlaneHeight;
double groundPlaneLightHeight = 10.0;
double groundPlaneSize = 15.0;
GLuint displayListGround;

// config file
string configFilename;
char renderingMeshFilename[4096];
char secondaryRenderingMeshFilename[4096];
char secondaryRenderingMeshInterpolationFilename[4096];
char volumetricMeshFilename[4096];
char customMassSpringSystem[4096];
char deformableObjectMethod[4096];
char fixedVerticesFilename[4096];
char massSpringSystemObjConfigFilename[4096];
char massSpringSystemTetMeshConfigFilename[4096];
char massSpringSystemCubicMeshConfigFilename[4096];
char invertibleMaterialString[4096] = "__none";
char initialPositionFilename[4096];
char initialVelocityFilename[4096];
char forceLoadsFilename[4096];
char outputFilename[4096];
int corotationalLinearFEM_warp = 1;
const int max_corotationalLinearFEM_warp = 2;
char implicitSolverMethod[4096];
char solverMethod[4096];
char extraSceneGeometryFilename[4096];
char lightingConfigFilename[4096];
float dampingMassCoef; // Rayleigh mass damping
float dampingStiffnessCoef; // Rayleigh stiffness damping
float dampingLaplacianCoef = 0.0; // Laplacian damping (rarely used)
float deformableObjectCompliance = 1.0; // scales all user forces by the provided factor

char fixedVerticesKVFFileName[4096];
char uDeformationoutputFileName[4096];
char ObjectVertexIndexInElement[4096];
char ExternFileDirectory[4096];
char ExternFileLineForceDirectory[4096];
char ExternStreePointsDirectory[4096];

// adjusts the stiffness of the object to cause all frequencies scale by the provided factor:
// keep it to 1.0 (except for experts)
float frequencyScaling = 1.0; 
int maxIterations; // for implicit integration
double epsilon; // for implicit integration
char backgroundColorString[4096] = "255 255 255";
int numInternalForceThreads;
int numSolverThreads;

// simulation
int syncTimestepWithGraphics=1;
float timeStep = 1.0 / 30;
float newmarkBeta = 0.25;
float newmarkGamma = 0.5;
int use1DNewmarkParameterFamily = 1;
int substepsPerTimeStep = 1;
double inversionThreshold;
double fps = 0.0;
AveragingBuffer fpsBuffer(5);
double cpuLoad = 0;
double forceAssemblyTime = 0.0;
AveragingBuffer forceAssemblyBuffer(50);
double systemSolveTime = 0.0;
AveragingBuffer systemSolveBuffer(50);
int enableTextures = 0;
int staticSolver = 0;
int graphicFrame = 0;
int lockAt30Hz = 0;
int forceNum = 3;

std::vector<int> LineExtraForces;
struct SPullVertexInfo
{
	int StemPullVertexNum;
	std::vector<int> PullVertexIndex;
	std::vector<int> StemExtraForces;
	std::vector<std::vector<int>> LeafExtraForces;
	std::vector<float> Scale;
    std::vector<std::pair<int, int>> DeviationDirection;
};
SPullVertexInfo pullVertexInfo;
int* pulledVertex = (int*)calloc(forceNum, sizeof(int));
double vForce[3];
int* constantpulledVertex = (int*)calloc(forceNum, sizeof(int));
int forceNeighborhoodSize = 5;
int dragStartX, dragStartY;
int explosionFlag = 0;
PerformanceCounter titleBarCounter;
PerformanceCounter explosionCounter;
PerformanceCounter cpuLoadCounter;
int timestepCounter = 0;
int subTimestepCounter = 0;
int numFixedVertices;
int * fixedVertices;

int numFixedKVFNumber;
int * fixedKVFVertices;
std::vector<int> KVFVertices;

std::vector<int> StemExtraForces;
std::vector<std::vector<int>> LeafExtraForces;
std::vector<int> TempExtraForces;
int FramesNumber = 0;
int Amplitude = 0;
int Frequency = 0;
int vYpluse = 0;
int Theta = 0;
int Phi = 0;

int numForceLoads = 0;
//加载力模型
double * forceLoads = nullptr;
IntegratorBase * integratorBase = nullptr;
ImplicitNewmarkSparse * implicitNewmarkSparse = nullptr;
IntegratorBaseSparse * integratorBaseSparse = nullptr;
ForceModel * forceModel = nullptr;
int enableCompressionResistance = 1;
double compressionResistance = 500;
int centralDifferencesTangentialDampingUpdateMode = 1;
int addGravity=0;
double g=9.81;

VolumetricMesh * volumetricMesh = nullptr;
TetMesh * tetMesh = nullptr;
Graph * meshGraph = nullptr;

StVKFEM * stVKFEM = nullptr;

enum massSpringSystemSourceType { OBJ, TETMESH, CUBICMESH, CHAIN, NONE } massSpringSystemSource = NONE;
enum deformableObjectType { STVK, COROTLINFEM, LINFEM, MASSSPRING, INVERTIBLEFEM, UNSPECIFIED } deformableObject = UNSPECIFIED;
enum invertibleMaterialType { INV_STVK, INV_NEOHOOKEAN, INV_MOONEYRIVLIN, INV_NONE } invertibleMaterial = INV_NONE;
enum solverType { IMPLICITNEWMARK, IMPLICITBACKWARDEULER, EULER, SYMPLECTICEULER, CENTRALDIFFERENCES, UNKNOWN } solver = UNKNOWN;

StencilForceModel * stencilForceModel = nullptr;
ForceModelAssembler *forceModelAssembler = nullptr;
CorotationalLinearFEMStencilForceModel * corotationalLinearFEMStencilForceModel = nullptr;
IsotropicHyperelasticFEMStencilForceModel * isotropicHyperelasticFEMStencilForceModel = nullptr;
MassSpringStencilForceModel * massSpringStencilForceModel = nullptr;
StVKStencilForceModel * stVKStencilForceModel = nullptr;
LinearFEMStencilForceModel * linearFEMStencilForceModel = nullptr;

MassSpringSystem * massSpringSystem = nullptr;
RenderSprings * renderMassSprings = nullptr;
SparseMatrix * massMatrix = nullptr;
SparseMatrix * LaplacianDampingMatrix = nullptr;

int n;
double allVerticesBaseForce = 300;
double * u = nullptr;
double * uvel = nullptr;
double * uaccel = nullptr;
double * f_ext = nullptr;
double * f_extBase = nullptr;
double * uSecondary = nullptr;
double * uInitial = nullptr;
double * velInitial = nullptr;

double * deltau = nullptr;
double * preu = nullptr;
double * deltaSecondaryu = nullptr;



// interpolation to secondary mesh
int secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices;
int * secondaryDeformableObjectRenderingMesh_interpolation_vertices = nullptr;
double * secondaryDeformableObjectRenderingMesh_interpolation_weights = nullptr;

// glui
GLUI * glui;
GLUI_Spinner * timeStep_spinner;
GLUI_StaticText * systemSolveStaticText;
GLUI_StaticText * forceAssemblyStaticText;

void callAllUICallBacks();
void Sync_GLUI();
void stopDeformations_buttonCallBack(int code);

//font is, for example, GLUT_BITMAP_9_BY_15
void print_bitmap_string(float x, float y, float z, void * font, char * s)
{
  glRasterPos3f(x,y,z);
  if (s && strlen(s)) 
  {
    while (*s) 
    {
      glutBitmapCharacter(font, *s);
      s++;
    }
  }
}

// renders the ground plane
void RenderGroundPlane(double groundPlaneHeight, double rPlane, double gPlane, double bPlane, double ambientFactor, double diffuseFactor, double specularFactor, double shininess)
{
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0,1.0);

  float planeAmbient[4] = { (float)(ambientFactor*rPlane), (float)(ambientFactor*gPlane), (float)(ambientFactor*bPlane), 1.0};
  float planeDiffuse[4] = { (float)(diffuseFactor*rPlane), (float)(diffuseFactor*gPlane), (float)(diffuseFactor*bPlane), 1.0};
  float planeSpecular[4] = { (float)(specularFactor*rPlane), (float)(specularFactor*gPlane), (float)(specularFactor*bPlane), 1.0};
  float planeShininess = shininess; 
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, planeAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, planeDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, planeSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, planeShininess);

  glNormal3f(0,1,0);
  const int planeResolution = 100;
  double planeIncrement = groundPlaneSize / planeResolution;
  for(int i=0; i<planeResolution; i++)
    for(int j=0; j<planeResolution; j++)
    {
      glBegin(GL_QUADS);
      glVertex3f(-groundPlaneSize/2 + i * planeIncrement, groundPlaneHeight, -groundPlaneSize/2 + j * planeIncrement);
      glVertex3f(-groundPlaneSize/2 + i * planeIncrement, groundPlaneHeight, -groundPlaneSize/2 + (j+1) * planeIncrement);
      glVertex3f(-groundPlaneSize/2 + (i+1) * planeIncrement, groundPlaneHeight, -groundPlaneSize/2 + (j+1) * planeIncrement);
      glVertex3f(-groundPlaneSize/2 + (i+1) * planeIncrement, groundPlaneHeight, -groundPlaneSize/2 + j * planeIncrement);
      glEnd();
    }
  glDisable(GL_POLYGON_OFFSET_FILL);
}

//渲染函数
// graphics loop function.
void displayFunction(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // setup model transformations
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();

  camera->Look();

  // set OpenGL lighting 
  deformableObjectRenderingMesh->SetLighting(lighting);

  glEnable(GL_LIGHTING);
  glPolygonOffset(0.0,0.0);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  glStencilFunc(GL_ALWAYS, 0, ~(0u));
  // render embedded triangle mesh 绘制每一帧的obj模型
  if (renderSecondaryDeformableObject)
    secondaryDeformableObjectRenderingMesh->Render();

  glStencilFunc(GL_ALWAYS, 1, ~(0u));

  // render the main deformable object (surface of volumetric mesh)
  if (renderDeformableObject)
  {
    if (renderSecondaryDeformableObject)
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);

      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1.0, 1.0);
      glDrawBuffer(GL_NONE);
      deformableObjectRenderingMesh->Render();
      glDisable(GL_POLYGON_OFFSET_FILL);
      glDrawBuffer(GL_BACK);
      glEnable(GL_LIGHTING);
    }

	//外部包围网格
   glColor3f(0.0, 0.0, 0.0);
   deformableObjectRenderingMesh->Render();

    if (renderVertices)
    {
      glDisable(GL_LIGHTING);
      glColor3f(0.5,0,0);
      glPointSize(8.0);
      deformableObjectRenderingMesh->RenderVertices();
      glEnable(GL_LIGHTING);
    }

    if (renderSecondaryDeformableObject)
    {
      glDisable(GL_BLEND);
    }
  }

  // render any extra scene geometry
  glStencilFunc(GL_ALWAYS, 0, ~(0u));
  if (extraSceneGeometry != nullptr)
    extraSceneGeometry->Render();

  double ground[4] = {0,1,0,-groundPlaneHeight-0.01};
  double light[4] = {0,groundPlaneLightHeight,0,1}; 

  glDisable(GL_TEXTURE_2D);

  // render shadow
  if (renderGroundPlane)
  {
    glColor3f(0.1,0.1,0.1);
    glDisable(GL_LIGHTING);
    deformableObjectRenderingMesh->RenderShadow(ground, light);

    if (extraSceneGeometry != nullptr)
      extraSceneGeometry->RenderShadow(ground, light);
    glEnable(GL_LIGHTING);
    glCallList(displayListGround);
  }

  glDisable(GL_LIGHTING);

  glStencilFunc(GL_ALWAYS, 1, ~(0u));
  glColor3f(0,0,0);

  //显示包围网格边框
  if (renderWireframe)
	deformableObjectRenderingMesh->RenderEdges();

  // disable stencil buffer modifications
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  glColor3f(0,0,0);

  if (renderAxes)
  {
	  glLineWidth(1.0);
	  drawAxes(100.0);
	  //drawLocationAxes(camera);
	  //draw2DAxes(camera);
	 
  }

  /*for (int i = 0;i< pullVertexInfo.PullVertexIndex.size(); i++)
  {
	  double constantpulledVertexPos[3];
	  deformableObjectRenderingMesh->GetSingleVertexPositionFromBuffer(pullVertexInfo.PullVertexIndex[i], &constantpulledVertexPos[0], &constantpulledVertexPos[1], &constantpulledVertexPos[2]);
	  drawForceDirection(constantpulledVertexPos, vForce, 1);
  }*/
	  

  // render the currently pulled vertex
 /*glColor3f(1, 0.8, 0);
 double pulledVertexPos[3];
 for (int i = 0;i< pullVertexInfo.PullVertexIndex.size(); i++)
 {
	 deformableObjectRenderingMesh->GetSingleVertexPositionFromBuffer(pullVertexInfo.PullVertexIndex[i], &pulledVertexPos[0], &pulledVertexPos[1], &pulledVertexPos[2]);
	  glEnable(GL_POLYGON_OFFSET_POINT);
	  glPolygonOffset(-1.0, -1.0);
	  glPointSize(18.0);
	  glBegin(GL_POINTS);
	  glVertex3f(pulledVertexPos[0], pulledVertexPos[1], pulledVertexPos[2]);
	  glEnd();
	  glDisable(GL_POLYGON_OFFSET_FILL);
 }*/


  // render model fixed vertices
  if (renderFixedVertices)
  {
    for(int i=0; i<numFixedVertices; i++)
    {
      glColor3f(1,0,0);
      double fixedVertexPos[3];
      deformableObjectRenderingMesh->GetSingleVertexRestPosition(fixedVertices[i],
          &fixedVertexPos[0], &fixedVertexPos[1], &fixedVertexPos[2]);

      glEnable(GL_POLYGON_OFFSET_POINT);
      glPolygonOffset(-1.0,-1.0);
      glPointSize(12.0);
      glBegin(GL_POINTS);
      glVertex3f(fixedVertexPos[0], fixedVertexPos[1], fixedVertexPos[2]);
      glEnd();
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
  }

  // render springs for mass-spring systems
  if ((massSpringSystem != nullptr) & renderSprings)
  { 
    printf("rendering springs\n");
    glLineWidth(2.0);
    renderMassSprings->Render(massSpringSystem, u);
    glLineWidth(1.0);
  } 

  // ==== bitmap routines below here
  glMatrixMode(GL_MODELVIEW); 
  glPushMatrix(); 
  glLoadIdentity(); 
  glMatrixMode(GL_PROJECTION); 
  glPushMatrix(); 
  glLoadIdentity(); 

  // print info in case of integrator blow-up
  char s[4096];
  glColor3f(1,0,0);
  if (explosionFlag)
  {
    sprintf(s,"The integrator went unstable.");
    double x1 = 10;
    double y1 = 25;
    double X1 = -1 + 2.0 * x1 / windowWidth;
    double Y1 = -1 + 2.0 * y1 / windowHeight;
    print_bitmap_string(X1,Y1,-1,GLUT_BITMAP_9_BY_15 ,s);

    sprintf(s,"Reduce the timestep, or increase the number of substeps per timestep.");
    x1 = 10;
    y1 = 10;
    X1 = -1 + 2.0 * x1 / windowWidth;
    Y1 = -1 + 2.0 * y1 / windowHeight;
    print_bitmap_string(X1,Y1,-1,GLUT_BITMAP_9_BY_15 ,s);
  }

  glPopMatrix(); 
  glMatrixMode(GL_MODELVIEW); 
  glPopMatrix(); 

  glutSwapBuffers();
}

void distributeForce(int vPullVertex, double * f_ext, double* externalForce)
{
	// distribute force over the neighboring vertices
	set<int> affectedVertices;
	set<int> lastLayerVertices;

	affectedVertices.insert(vPullVertex);
	lastLayerVertices.insert(vPullVertex);

	//力影响周围的点数量
	for (int j = 1; j < forceNeighborhoodSize; j++)
	{
		// linear kernel
		  //对外围点的影响力，对外围每层受力每次递减百分之20
		double forceMagnitude = 1.0 * (forceNeighborhoodSize - j) / forceNeighborhoodSize;

		set<int> newAffectedVertices;
		for (set<int> ::iterator iter = lastLayerVertices.begin(); iter != lastLayerVertices.end(); iter++)
		{
			// traverse all neighbors and check if they were already previously inserted
			int vtx = *iter;
			int deg = meshGraph->GetNumNeighbors(vtx);
			for (int k = 0; k < deg; k++)
			{
				int vtxNeighbor = meshGraph->GetNeighbor(vtx, k);

				if (affectedVertices.find(vtxNeighbor) == affectedVertices.end())
				{
					// discovered new vertex
					newAffectedVertices.insert(vtxNeighbor);
				}
			}
		}

		lastLayerVertices.clear();
		for (set<int> ::iterator iter = newAffectedVertices.begin(); iter != newAffectedVertices.end(); iter++)
		{
			// apply force
			f_ext[3 * *iter + 0] += forceMagnitude * externalForce[0];
			f_ext[3 * *iter + 1] += forceMagnitude * externalForce[1];
			f_ext[3 * *iter + 2] += forceMagnitude * externalForce[2];

			// generate new layers
			lastLayerVertices.insert(*iter);
			affectedVertices.insert(*iter);
		}
	}
}

// called periodically by GLUT:
void idleFunction(void)
{
  cpuLoadCounter.StartCounter();

  glutSetWindow(windowID);

  // reset external forces (usually to zero)
  memcpy(f_ext, f_extBase, sizeof(double) * 3 * n);

  if (subTimestepCounter == 0)
  {
	  /*CModelDeformationTransform tempdeformationsave;
	  tempdeformationsave.SaveDeformationVertexFromBaseModel(deltaSecondaryu, secondaryDeformableObjectRenderingMesh->GetNumVertices(), uDeformationoutputFileName, subTimestepCounter);*/
	  memcpy(deltau, integratorBase->Getq(), sizeof(double) * 3 * n);
	  memcpy(preu, integratorBase->Getq(), sizeof(double) * 3 * n);
  }

  if ((!lockScene) && (!pauseSimulation) && (singleStepMode <= 1))
  {
#pragma region pullVertices 
	  // determine force in case user is pulling on a vertex
	  /*if (g_iLeftMouseButton)
	  {
		if (pulledVertex != -1)
		{*/
		//stem	
	 /* pulledVertex[0] = 6172;


	  pulledVertex[1] = 6552;
	   pulledVertex[2] = 4214;*/
	  /*pulledVertex[4] = 12203;
	  pulledVertex[5] = 2768;
	  pulledVertex[6] = 12015;
	  pulledVertex[7] = 4625;
	  pulledVertex[8] = 9529;*/

	  /* double forceX =99;
	   double forceY = -49;*/
	   /*double forceX = (g_vMousePos[0] - dragStartX);
		 double forceY = -(g_vMousePos[1] - dragStartY);*/
	 
	  double externalForce[3];

	  //计算外力
	 /* camera->CameraVector2WorldVector_OrientationOnly3D(
		  forceX, forceY, 0, externalForce);*/
	 // camera->setWorldCoorinateSystemForce(StemExtraForces[subTimestepCounter], 0, 0, externalForce);
#pragma endregion
	  

	  //stem
	  for (int i = 0; i < pullVertexInfo.StemPullVertexNum; i++)
	  {
		  camera->setWorldCoorinateSystemForce(pullVertexInfo.StemExtraForces[subTimestepCounter], Theta, Phi, externalForce);
		  std::copy(externalForce, externalForce + 3, vForce);
		  for (int j = 0; j < 3; j++)
		  {
			  externalForce[j] *= deformableObjectCompliance * pullVertexInfo.Scale[i];
			  f_ext[3 * pullVertexInfo.PullVertexIndex[i] + j] += externalForce[j];
		  }
		  //distribute force over the neighboring vertices
		  distributeForce(pullVertexInfo.PullVertexIndex[i], f_ext, externalForce);
	  }

	  //leaf

	  for (int i = pullVertexInfo.StemPullVertexNum; i < pullVertexInfo.PullVertexIndex.size(); i++)
	  {
		  camera->setWorldCoorinateSystemForce(pullVertexInfo.LeafExtraForces[i-pullVertexInfo.StemPullVertexNum][subTimestepCounter], Theta+ pullVertexInfo.DeviationDirection[i].first, Phi+ pullVertexInfo.DeviationDirection[i].second, externalForce);
		  //std::copy(externalForce, externalForce + 3, vForce);
		  for (int j = 0; j < 3; j++)
		  {
			  externalForce[j] *= deformableObjectCompliance * pullVertexInfo.Scale[i];
			  f_ext[3 * pullVertexInfo.PullVertexIndex[i] + j] += externalForce[j];
		  }
		  //distribute force over the neighboring vertices
		  distributeForce(pullVertexInfo.PullVertexIndex[i], f_ext, externalForce);
	  }
	 

	  //printf("%d fx: %G fy: %G | %G %G %G\n", pulledVertex, forceX, forceY, externalForce[0], externalForce[1], externalForce[2]);

	  // register force on the pulled vertex
	  //for (int i = 0; i < pullVertexInfo.PullVertexNum; i++)
	  //{
		 // for (int j = 0; j < 3; j++)
		 // {
			//  externalForce[j] *= deformableObjectCompliance * scale[i];
			//  f_ext[3 * pulledVertex[i] + j] += externalForce[j];
		 // }
		 // //distribute force over the neighboring vertices
		 // distributeForce(pulledVertex[i], f_ext, externalForce);
	  //}
		
      /*}
    }*/

    // apply any scripted force loads
    if (timestepCounter < numForceLoads)
    {
      printf("  External forces read from the binary input file.\n");
      for(int i=0; i<3*n; i++)
        f_ext[i] += forceLoads[ELT(3*n, i, timestepCounter)];
    }

    // set forces to the integrator
    integratorBaseSparse->SetExternalForces(f_ext);

    PerformanceCounter totalDynamicsCounter;

	
    // timestep the dynamics 
    for(int i=0; i<substepsPerTimeStep; i++)
    {
		TempExtraForces.push_back(StemExtraForces[subTimestepCounter]);
		if ((subTimestepCounter+1) % Common::ForcesSampling == 0)
		{
			//integratorBase->WriteSpecificKRFextVMattixToFile(outputFilename, subTimestepCounter, KVFVertices, TempExtraForces);
			TempExtraForces.clear();
		}
		//计算由力产生的结点位移形变
      int code = integratorBase->DoTimestep();
	  /*std::vector<int> tempvec = { 0,4,1677 };*/

	  /*integratorBase->WriteKRFextVMartixToFile(outputFilename, subTimestepCounter);*/

      //printf(".");
    	fflush(nullptr);

      double forceAssemblyLocalTime = integratorBaseSparse->GetForceAssemblyTime();
      double systemSolveLocalTime = integratorBaseSparse->GetSystemSolveTime();
      //printf("Force assembly: %G\nSystem solve: %G\n", forceAssemblyTime, systemSolveTime);

      forceAssemblyBuffer.addValue(forceAssemblyLocalTime);
      forceAssemblyTime = forceAssemblyBuffer.getAverage();

      systemSolveBuffer.addValue(systemSolveLocalTime);
      systemSolveTime = systemSolveBuffer.getAverage();

      if (code != 0)
      {
        printf("The integrator went unstable. Reduce the timestep, or increase the number of substeps per timestep.\n");
        integratorBase->ResetToRest();
        for(int i=0; i<3*n; i++)
        {
          f_ext[i] = 0;
          f_extBase[i] = 0;
        }
        integratorBase->SetExternalForcesToZero();
        explosionFlag = 1;
        explosionCounter.StartCounter();
        break;
      }
	  subTimestepCounter++;
    }

	//std::cout << subTimestepCounter << std::endl;

	if (subTimestepCounter > 180)
	{
		exit(1);
	}
	//用于判断文件力的数量
    //timestepCounter++;

    totalDynamicsCounter.StopCounter();
    //printf("Total dynamics: %G\n", totalDynamicsCounter.GetElapsedTime());

    memcpy(u, integratorBase->Getq(), sizeof(double) * 3 * n);

	//计算每一帧产生的delta形变
	for (int i = 0; i < 3 * n; i++)
	{
		deltau[i] = u[i] - preu[i];
	}
	memcpy(preu, integratorBase->Getq(), sizeof(double) * 3 * n);
	
    if (singleStepMode == 1)
      singleStepMode = 2;

    //printf("F");





  	fflush(nullptr);
    graphicFrame++;

    if (lockAt30Hz)
    {
      double elapsedTime;
      do
      {
        titleBarCounter.StopCounter();
        elapsedTime = titleBarCounter.GetElapsedTime();
      }
      while (1.0 * graphicFrame / elapsedTime >= 30.0);
    }
  }
  //外部的六面体网格发生位移形变(对顶点进行位移)
  deformableObjectRenderingMesh->SetVertexDeformations(u);
 

  // interpolate deformations from volumetric mesh to rendering triangle mesh
  if (secondaryDeformableObjectRenderingMesh != nullptr)
  {
    PerformanceCounter interpolationCounter;

    VolumetricMesh::interpolate(u, uSecondary, secondaryDeformableObjectRenderingMesh->Getn(), secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices, secondaryDeformableObjectRenderingMesh_interpolation_vertices, secondaryDeformableObjectRenderingMesh_interpolation_weights);
	//内部的三角形网格发生形变
    secondaryDeformableObjectRenderingMesh->SetVertexDeformations(uSecondary);

	
	VolumetricMesh::interpolate(deltau, deltaSecondaryu, secondaryDeformableObjectRenderingMesh->Getn(), secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices, secondaryDeformableObjectRenderingMesh_interpolation_vertices, secondaryDeformableObjectRenderingMesh_interpolation_weights);
	//////////////////////
	ObjMesh * mesh = secondaryDeformableObjectRenderingMesh->GetMesh();
	//double * restPosition = secondaryDeformableObjectRenderingMesh->GetVertexRestPositions();
	CModelDeformationTransform deformationsave;
	//deformationsave.SaveDeformationVertexFromBaseModel(uSecondary, secondaryDeformableObjectRenderingMesh->GetNumVertices(), outputFilename, subTimestepCounter);

	//每uDeformationSampling帧存储最终的形变数据U
	/*if (subTimestepCounter % Common::uDeformationSampling == 0)
	{
		deformationsave.SaveDeformationVertexFromBaseModel(uSecondary, secondaryDeformableObjectRenderingMesh->GetNumVertices(), uDeformationoutputFileName, subTimestepCounter);
		TempExtraForces.clear();
	}*/
	//存储deltaU的形变数据
	//deformationsave.SaveDeformationVertexFromBaseModel(deltaSecondaryu, secondaryDeformableObjectRenderingMesh->GetNumVertices(), outputFilename, subTimestepCounter-1);

    //存储U的形变数据
	/*if (subTimestepCounter == 2000)
	{
		deformationsave.SaveDeformationVertexFromBaseModel(uSecondary, secondaryDeformableObjectRenderingMesh->GetNumVertices(), outputFilename, subTimestepCounter - 1);
	}*/
	

    if (subTimestepCounter % 5 == 0)
    {
        std::string filePath = "D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/MoveObj/";
        filePath += std::to_string(subTimestepCounter);
        filePath += ".obj";
        mesh->saveToAscii(filePath, 1);
    }
    //mesh->saveToAscii("D:/GraduationProject/Vega/models/8.10/position/1.obj", 1);
	//CSence sence(mesh, restPosition);

	//std::vector<std::vector<glm::vec3>> data = sence.getGroupDeformationData();
	//if (strcmp(outputFilename, "__none") != 0)
	//{
	//	char s[4096];
	//	//写入可查看的位移文件
	////sprintf(s, "%s.u.%04d.txt", "P", subTimestepCounter);
	//	FILE * file = fopen(outputFilename, "a");
	//	//printf("Saving deformation to %s.\n", s);
	//	if (!file)
	//	{
	//		printf("Can't open output file: %s.\n", s);
	//	}
	//	else
	//	{
	//		sprintf(s, "Position%04d", subTimestepCounter);
	//		fprintf(file, "%s \n", s);
	//		for (auto i = 0; i < data.size(); i++)
	//		{
	//			sprintf(s, "group%04d", i);
	//			fprintf(file, "%s \n", s);
	//			for (auto j = 0; j < data[i].size(); j++)
	//			{
	//				std::vector<glm::vec3> tempdata=data[i];
	//				fprintf(file, "%.10lf %.10lf %10.lf ", tempdata[j].x,tempdata[j].y,tempdata[j].z);
	//			}
	//		}
	//		fprintf(file, "\n");
	//	}
	//	fclose(file);
	//}

    interpolationCounter.StopCounter();
    //printf("Interpolate deformations: %G\n", interpolationCounter.GetElapsedTime());
  }

  if (useRealTimeNormals)
  {
    // recompute normals
    PerformanceCounter normalsCounter;
    deformableObjectRenderingMesh->BuildNormals(); 
    if (secondaryDeformableObjectRenderingMesh != nullptr)
      secondaryDeformableObjectRenderingMesh->BuildNormals();
    normalsCounter.StopCounter();
    //printf("Recompute normals: %G\n", normalsCounter.GetElapsedTime());
  }

  if (explosionFlag)
  {
    explosionCounter.StopCounter();
    if (explosionCounter.GetElapsedTime() > 4.0) // the message will appear on screen for 4 seconds
      explosionFlag = 0;
  }

  // update window title at 5 Hz
  titleBarCounter.StopCounter();
  double elapsedTime = titleBarCounter.GetElapsedTime();
  if (elapsedTime >= 1.0 / 5)
  {
    titleBarCounter.StartCounter();
    double fpsLocal = graphicFrame / elapsedTime;
    fpsBuffer.addValue(fpsLocal);
    fps = fpsBuffer.getAverage();

    //printf("Frames per second: %G\n", fps);

    // update window title 
    char windowTitle[4096] = "unknown defo model";

    if (deformableObject == STVK)
      sprintf(windowTitle,"%s | %s | Elements: %d | DOFs: %d | %.1f Hz | Defo CPU Load: %d%%", "StVK", solverMethod, volumetricMesh->getNumElements(), volumetricMesh->getNumVertices() * 3, fps, (int)(100 * cpuLoad + 0.5) );

    if (deformableObject == COROTLINFEM)
      sprintf(windowTitle,"%s:%d | %s | Elements: %d | DOFs: %d | %.1f Hz | Defo CPU Load: %d%% | Fps: %d", "CLFEM", corotationalLinearFEM_warp, solverMethod, volumetricMesh->getNumElements(), volumetricMesh->getNumVertices() * 3, fps, (int)(100 * cpuLoad + 0.5), subTimestepCounter);

    if (deformableObject == LINFEM)
      sprintf(windowTitle,"%s | %s | Elements: %d | DOFs: %d | %.1f Hz | Defo CPU Load: %d%%", "LinFEM", solverMethod, volumetricMesh->getNumElements(), volumetricMesh->getNumVertices() * 3, fps, (int)(100 * cpuLoad + 0.5) );

    if (deformableObject == MASSSPRING)
      sprintf(windowTitle,"%s | %s | Particles: %d | Edges: %d | %.1f Hz | Defo CPU Load: %d%%", "mass-spring", solverMethod, massSpringSystem->GetNumParticles(), massSpringSystem->GetNumEdges(), fps, (int)(100 * cpuLoad + 0.5) );

    if (deformableObject == INVERTIBLEFEM)
    {
      char materialType[96] = "Invertible FEM";

      if (invertibleMaterial == INV_STVK)
        strcpy(materialType, "Invertible StVK");

      if (invertibleMaterial == INV_NEOHOOKEAN)
        strcpy(materialType, "Invertible neo-Hookean");

      if (invertibleMaterial == INV_MOONEYRIVLIN)
        strcpy(materialType, "Invertible Mooney-Rivlin");

      sprintf(windowTitle,"%s | %s | Elements: %d | DOFs: %d | %.1f Hz | Defo CPU Load: %d%%", materialType, solverMethod, volumetricMesh->getNumElements(), volumetricMesh->getNumVertices() * 3, fps, (int)(100 * cpuLoad + 0.5) );
    }

    glutSetWindowTitle(windowTitle);
    graphicFrame = 0;

    char ptext1[96];
    sprintf(ptext1, "Force assembly: %G", forceAssemblyTime);
    forceAssemblyStaticText->set_text(ptext1);
    char solverName[96];
    GetIntegratorSolver(solverName);
    char ptext2[96];
    sprintf(ptext2, "System solve (%s): %G", solverName, systemSolveTime);
    systemSolveStaticText->set_text(ptext2);
    Sync_GLUI();

    if ((syncTimestepWithGraphics) && ((!lockScene) && (!pauseSimulation) && (singleStepMode == 0)))
    {
      timeStep = 1.0 / fps;
      integratorBase->SetTimestep(timeStep / substepsPerTimeStep);
      Sync_GLUI();
    }
  }

  cpuLoadCounter.StopCounter();
  double cpuTimePerGraphicsFrame = cpuLoadCounter.GetElapsedTime();
  
  cpuLoad = cpuTimePerGraphicsFrame * fps; 
  //printf("cpuTimePerGraphicsFrame: %lf\n", cpuTimePerGraphicsFrame);
  glutPostRedisplay();
}

// reacts to pressed keys
void keyboardFunction(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:
      exit(0);

    case 13:
      if (singleStepMode == 2)
        singleStepMode = 1;
      break;

    case 'w':
      renderWireframe = !renderWireframe;
      break;

    case 'i':
    {
      double focusPos[3];
      camera->GetFocusPosition(focusPos);
      double cameraX,cameraY,cameraZ;
      camera->GetAbsWorldPosition(cameraX,cameraY,cameraZ);
      printf("Camera is positioned at: %G %G %G\n",cameraX,cameraY,cameraZ);
      printf("Camera radius is: %G\n",camera->GetRadius());
      printf("Camera Phi is: %G\n",180.0/M_PI*camera->GetPhi());
      printf("Camera Theta is: %G\n",180.0/M_PI*camera->GetTheta());
      printf("Camera focus is: %G %G %G\n", focusPos[0], focusPos[1], focusPos[2]);
    }
    break;

    case '\\':
      camera->Reset();
      break;

    case 'a':
      renderAxes = !renderAxes;
      break;

    case 'b':
      renderFixedVertices = !renderFixedVertices;
      break;

    case 'v':
      renderSprings = !renderSprings;
      break;

    case 'V':
      renderVertices = !renderVertices;
      break;

    case 'L':
      lockAt30Hz = !lockAt30Hz;
      break;

      // in this mode, can move the camera while the object's deformations are frozen
    case 'l':
      lockScene = !lockScene;
      if (lockScene)
      {
        camera->PushPosition();
      }
      else
      {
        camera->PopPosition();
      }
      break;

    case '1':
      corotationalLinearFEM_warp = (corotationalLinearFEM_warp + 1) % (max_corotationalLinearFEM_warp + 1);
      if(corotationalLinearFEMStencilForceModel != nullptr)
        corotationalLinearFEMStencilForceModel->SetWarp(corotationalLinearFEM_warp);

      printf("CorotationalLinearFEM warp is now: %d\n", corotationalLinearFEM_warp);
      break;

    case 'e':
      renderDeformableObject = !renderDeformableObject;
      break;

    case 'E':
      renderSecondaryDeformableObject = !renderSecondaryDeformableObject;
      if (secondaryDeformableObjectRenderingMesh == nullptr)
        renderSecondaryDeformableObject = 0;
      break;

    case 'N':
      useRealTimeNormals = !useRealTimeNormals;
      break;

    case 'p':
      pauseSimulation = !pauseSimulation;
      singleStepMode = 0;
      break;

    case 'P':
      if (singleStepMode > 0)
	singleStepMode = 0;
      else
	singleStepMode = 1;
      break;

    case 'u':
      // save the current deformation to a disk file
      printf("Writing defomations to out.u .\n");
      WriteMatrixToDisk("out.u", 3*n, 1, u);
      break;

    case 'z':
      for(int i=0; i<3*n; i++)
        f_extBase[i] = f_ext[i];
      dragStartX = g_vMousePos[0];
      dragStartY = g_vMousePos[1];
      break;

    case 'Z':
      for(int i=0; i<3*n; i++)
        f_extBase[i] = 0;
      dragStartX = g_vMousePos[0];
      dragStartY = g_vMousePos[1];
      break;

    case '0':
      stopDeformations_buttonCallBack(0);
      break;
  }
}

// reacts to pressed "special" keys
void specialFunction(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_LEFT:
      camera->MoveFocusRight(+0.1 * fabs(camera->GetRadius()));
    break;

    case GLUT_KEY_RIGHT:
      camera->MoveFocusRight(-0.1 * fabs(camera->GetRadius()));
    break;

    case GLUT_KEY_DOWN:
      camera->MoveFocusUp(+0.1 * fabs(camera->GetRadius()));
    break;

    case GLUT_KEY_UP:
      camera->MoveFocusUp(-0.1 * fabs(camera->GetRadius()));
    break;

    case GLUT_KEY_PAGE_UP:
      break;

    case GLUT_KEY_PAGE_DOWN:
      break;

    case GLUT_KEY_HOME:
      break;

    case GLUT_KEY_END:
      break;

    case GLUT_KEY_INSERT:
      break;

    default:
      break;
  }
}

void reshape(int x, int y)
{
  glViewport(0,0,x,y);

  windowWidth = x;
  windowHeight = y;

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 

  // compute the window aspect ratio 
  gluPerspective(45.0f, 1.0 * windowWidth / windowHeight, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// reacts to mouse motion
void mouseMotionFunction(int x, int y)
{
  int mouseDeltaX = x-g_vMousePos[0];
  int mouseDeltaY = y-g_vMousePos[1];

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;

  if (g_iLeftMouseButton) // left mouse button 
  {
  }

  if (g_iRightMouseButton) // right mouse button handles camera rotations
  {
    const double factor = 0.2;

    camera->MoveRight(factor * mouseDeltaX);
    camera->MoveUp(factor * mouseDeltaY);
  }

  if ((g_iMiddleMouseButton) || (g_iLeftMouseButton && altPressed)) // handle zoom in/out
  {
    const double factor = 0.1;
    camera->ZoomIn(cameraRadius * factor * mouseDeltaY);
  }
}

// reacts to pressed mouse buttons
void mouseButtonActivityFunction(int button, int state, int x, int y)
{
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      shiftPressed = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
      altPressed = (glutGetModifiers() == GLUT_ACTIVE_ALT);
      ctrlPressed = (glutGetModifiers() == GLUT_ACTIVE_CTRL);

      if ((g_iLeftMouseButton) && (!shiftPressed) && (!ctrlPressed))
      {
        // apply force to vertex

        GLdouble model[16];
        glGetDoublev (GL_MODELVIEW_MATRIX, model);

        GLdouble proj[16];
        glGetDoublev (GL_PROJECTION_MATRIX, proj);

        GLint view[4];
        glGetIntegerv (GL_VIEWPORT, view);

        int winX = x;
        int winY = view[3]-1-y;

        float zValue;
        glReadPixels(winX,winY,1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &zValue);

        GLubyte stencilValue;
        glReadPixels(winX, winY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &stencilValue);

        GLdouble worldX, worldY, worldZ;
        gluUnProject(winX, winY, zValue, model, proj, view, &worldX, &worldY, &worldZ);

        if (stencilValue == 1)
        {
          dragStartX = x;
          dragStartY = y;
          Vec3d pos(worldX, worldY, worldZ);
          pulledVertex[0] = deformableObjectRenderingMesh->GetClosestVertex(pos);
          printf("Clicked on vertex: %d (0-indexed)\n", pulledVertex);
        }
        else
        {
          printf("Clicked on empty stencil: %d.\n", stencilValue);
        }
      }

      if (!g_iLeftMouseButton)
      {
        pulledVertex[0] = -1;
      }

      break;

    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      shiftPressed = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
      ctrlPressed = (glutGetModifiers() == GLUT_ACTIVE_CTRL);
      break;

    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      shiftPressed = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
      ctrlPressed = (glutGetModifiers() == GLUT_ACTIVE_CTRL);
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

// program initialization
void initSimulation()
{
  // init lighting
  try
  {
    lighting = new Lighting(lightingConfigFilename);
  }
  catch(int exceptionCode)
  {
    printf("Error (%d) reading lighting information from %s .\n",exceptionCode, lightingConfigFilename);
    exit(1);
  }

  // init camera
  delete(camera);
  double virtualToPhysicalPositionFactor = 1.0;
  initCamera(cameraRadius, cameraLongitude, cameraLattitude,
     focusPositionX, focusPositionY, focusPositionZ,
     1.0 / virtualToPhysicalPositionFactor,
     &zNear, &zFar, &camera);

  volumetricMesh = nullptr;
  massSpringSystem = nullptr;

  // set deformable material type
  if (strcmp(volumetricMeshFilename, "__none") != 0)
  {
    if (strcmp(deformableObjectMethod, "StVK") == 0)
      deformableObject = STVK;
    if (strcmp(deformableObjectMethod, "CLFEM") == 0)
      deformableObject = COROTLINFEM;
    if (strcmp(deformableObjectMethod, "LinearFEM") == 0)
      deformableObject = LINFEM;
    if (strcmp(deformableObjectMethod, "InvertibleFEM") == 0)
      deformableObject = INVERTIBLEFEM;
  }

  if (strcmp(massSpringSystemObjConfigFilename, "__none") != 0)
    massSpringSystemSource = OBJ;
  else if (strcmp(massSpringSystemTetMeshConfigFilename, "__none") != 0)
    massSpringSystemSource = TETMESH;
  else if (strcmp(massSpringSystemCubicMeshConfigFilename, "__none") != 0)
    massSpringSystemSource = CUBICMESH;
  else if (strncmp(customMassSpringSystem, "chain", 5) == 0)
    massSpringSystemSource = CHAIN;

  if ((massSpringSystemSource == OBJ) || (massSpringSystemSource == TETMESH) || (massSpringSystemSource == CUBICMESH) || (massSpringSystemSource == CHAIN)) 
    deformableObject = MASSSPRING;

  if (deformableObject == UNSPECIFIED)
  {
    printf("Error: no deformable model specified.\n");
    exit(1);
  }

  // load volumetric mesh
  if ((deformableObject == STVK) || (deformableObject == COROTLINFEM) || (deformableObject == LINFEM) || (deformableObject == INVERTIBLEFEM))
  {
    printf("Loading volumetric mesh from file %s...\n", volumetricMeshFilename);

    VolumetricMesh::fileFormatType fileFormat = VolumetricMesh::ASCII;
    int verbose = 0;
    volumetricMesh = VolumetricMeshLoader::load(volumetricMeshFilename, fileFormat, verbose);

	/*******************将固定结点转换为固定element*********************/
	//volumetricMesh->checkPoint4Element("D:/GraduationProject/Vega/models/newgrass/voxelizegrass/vertexindex.txt", "D:/GraduationProject/Vega/models/newgrass/voxelizegrass/elementindex.txt");
   
	if (volumetricMesh == nullptr)
    {
      printf("Error: unable to load the volumetric mesh from %s.\n", volumetricMeshFilename);
      exit(1);
    }

    n = volumetricMesh->getNumVertices();
    printf("Num vertices: %d. Num elements: %d\n", n, volumetricMesh->getNumElements());
    meshGraph = GenerateMeshGraph::Generate(volumetricMesh);

    // create the mass matrix
    int inflate3Dim = true; // necessary so that the returned matrix is 3n x 3n
	//质量矩阵依靠材料密度进行计算
    GenerateMassMatrix::computeMassMatrix(volumetricMesh, &massMatrix, inflate3Dim);

    // create the internal forces for STVK and linear FEM materials
    if (deformableObject == STVK || deformableObject == LINFEM)  // LINFEM is constructed from stVKInternalForces
    {
      unsigned int loadingFlag = 0; // 0 = use the low-memory version, 1 = use the high-memory version
      StVKElementABCD * precomputedIntegrals = StVKElementABCDLoader::load(volumetricMesh, loadingFlag);
      if (precomputedIntegrals == nullptr)
      {
        printf("Error: unable to load the StVK integrals.\n");
        exit(1);
      }
	 
      printf("Generating internal forces and stiffness matrix models...\n"); fflush(nullptr);
      stVKFEM = new StVKFEM(volumetricMesh, precomputedIntegrals, addGravity, g);
    }
  }

  // load mass spring system (if any) 
  if (deformableObject == MASSSPRING)
  {
    switch (massSpringSystemSource)
    {
    case OBJ:
    {
      printf("Loading mass spring system from an obj file...\n");
      MassSpringSystemFromObjMeshConfigFile massSpringSystemFromObjMeshConfigFile;
      MassSpringSystemObjMeshConfiguration massSpringSystemObjMeshConfiguration;
      if (massSpringSystemFromObjMeshConfigFile.GenerateMassSpringSystem(massSpringSystemObjConfigFilename, &massSpringSystem, &massSpringSystemObjMeshConfiguration) != 0)
      {
        printf("Error initializing the mass spring system.\n");
        exit(1);
      }
      strcpy(renderingMeshFilename, massSpringSystemObjMeshConfiguration.massSpringMeshFilename);
    }
    break;

    case TETMESH:
    {
      printf("Loading mass spring system from a tet mesh file...\n");
      MassSpringSystemFromTetMeshConfigFile massSpringSystemFromTetMeshConfigFile;
      MassSpringSystemTetMeshConfiguration massSpringSystemTetMeshConfiguration;
	  //获取turtle.massspring
      if (massSpringSystemFromTetMeshConfigFile.GenerateMassSpringSystem(massSpringSystemTetMeshConfigFilename, &massSpringSystem, &massSpringSystemTetMeshConfiguration) != 0)
      {
        printf("Error initializing the mass spring system.\n");
        exit(1);
      }
      strcpy(renderingMeshFilename, massSpringSystemTetMeshConfiguration.surfaceMeshFilename);
    }
    break;

    case CUBICMESH:
    {
      printf("Loading mass spring system from a cubic mesh file...\n");
      MassSpringSystemFromCubicMeshConfigFile massSpringSystemFromCubicMeshConfigFile;
      MassSpringSystemCubicMeshConfiguration massSpringSystemCubicMeshConfiguration;
      if (massSpringSystemFromCubicMeshConfigFile.GenerateMassSpringSystem(massSpringSystemCubicMeshConfigFilename, &massSpringSystem, &massSpringSystemCubicMeshConfiguration) != 0)
      {
        printf("Error initializing the mass spring system.\n");
        exit(1);
      }
      strcpy(renderingMeshFilename, massSpringSystemCubicMeshConfiguration.surfaceMeshFilename);
    }
    break;

    case CHAIN:
    {
      int numParticles;
      double groupStiffness;
      sscanf(customMassSpringSystem, "chain,%d,%lf", &numParticles, &groupStiffness);
      printf("Creating a chain mass-spring system with %d particles...\n", numParticles);

      double * masses = (double*) malloc (sizeof(double) * numParticles);
      for(int i=0; i<numParticles; i++)
        masses[i] = 1.0;

      double * restPositions = (double*) malloc (sizeof(double) * 3 * numParticles);
      for(int i=0; i<numParticles; i++)
      {
        restPositions[3*i+0] = 0;
        restPositions[3*i+1] = (numParticles == 1) ? 0.0 : 1.0 * i / (numParticles-1);
        restPositions[3*i+2] = 0;
      }
      int * edges = (int*) malloc (sizeof(int) * 2 * (numParticles - 1));
      for(int i=0; i<numParticles-1; i++)
      {
        edges[2*i+0] = i;
        edges[2*i+1] = i+1;
      }

      int * edgeGroups = (int*) malloc (sizeof(int) * (numParticles - 1));
      for(int i=0; i<numParticles-1; i++)
        edgeGroups[i] = 0;
      double groupDamping = 0;

      massSpringSystem = new MassSpringSystem(numParticles, masses, restPositions, numParticles - 1, edges, edgeGroups, 1, &groupStiffness, &groupDamping, addGravity);

      char s[96];
      sprintf(s,"chain-%d.obj", numParticles);
      massSpringSystem->CreateObjMesh(s);
      strcpy(renderingMeshFilename, s);

      free(edgeGroups);
      free(edges);
      free(restPositions);
      free(masses);

      renderVertices = 1;
    }
    break;

    default:
      printf("Error: mesh spring system configuration file was not specified.\n");
      exit(1);
      break;
    }

    if (addGravity)
      massSpringSystem->SetGravity(addGravity, g);

    n = massSpringSystem->GetNumParticles();

    // create the mass matrix
    massSpringSystem->GenerateMassMatrix(&massMatrix);

    // create the mesh graph (used only for the distribution of user forces over neighboring vertices)
    meshGraph = new Graph(massSpringSystem->GetNumParticles(), massSpringSystem->GetNumEdges(), massSpringSystem->GetEdges());
  }

  int scaleRows = 1;
  meshGraph->GetLaplacian(&LaplacianDampingMatrix, scaleRows);
  LaplacianDampingMatrix->ScalarMultiply(dampingLaplacianCoef);

  // initialize the rendering mesh for the volumetric mesh
  //加载包围网格的obj模式voxelizesurfacegrass.obj
  if (strcmp(renderingMeshFilename, "__none") == 0)
  {
    printf("Error: rendering mesh was not specified.\n");
    exit(1);
  }
  deformableObjectRenderingMesh = new SceneObjectDeformable(renderingMeshFilename);
  if (enableTextures)
    deformableObjectRenderingMesh->SetUpTextures(SceneObject::MODULATE, SceneObject::NOMIPMAP);
  deformableObjectRenderingMesh->ResetDeformationToRest();
  deformableObjectRenderingMesh->BuildNeighboringStructure();
  deformableObjectRenderingMesh->BuildNormals(); 
  deformableObjectRenderingMesh->SetMaterialAlpha(0.5);

  // initialize the embedded triangle rendering mesh 
  //加载原obj文件
  secondaryDeformableObjectRenderingMesh = nullptr;
  if (strcmp(secondaryRenderingMeshFilename, "__none") != 0)
  {
    secondaryDeformableObjectRenderingMesh = new SceneObjectDeformable(secondaryRenderingMeshFilename);
    if (enableTextures)
      secondaryDeformableObjectRenderingMesh->SetUpTextures(SceneObject::MODULATE, SceneObject::NOMIPMAP);
    secondaryDeformableObjectRenderingMesh->ResetDeformationToRest();
    secondaryDeformableObjectRenderingMesh->BuildNeighboringStructure();
    secondaryDeformableObjectRenderingMesh->BuildNormals(); 

    uSecondary = (double*) calloc (3 * secondaryDeformableObjectRenderingMesh->Getn(), sizeof(double));

	deltaSecondaryu= (double*)calloc(3 * secondaryDeformableObjectRenderingMesh->Getn(), sizeof(double));

    // load interpolation structure
	//加载插值结构
    if (strcmp(secondaryRenderingMeshInterpolationFilename, "__none") == 0)
    {
      printf("Error: no secondary rendering mesh interpolation filename specified.\n");
      exit(1);
    }

    secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices = VolumetricMesh::getNumInterpolationElementVertices(secondaryRenderingMeshInterpolationFilename);

    if (secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices < 0)
    {
      printf("Error: unable to open file %s.\n", secondaryRenderingMeshInterpolationFilename);
      exit(1);
    }

    printf("Num interpolation element vertices: %d\n", secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices);

    VolumetricMesh::loadInterpolationWeights(secondaryRenderingMeshInterpolationFilename, secondaryDeformableObjectRenderingMesh->Getn(), secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices, &secondaryDeformableObjectRenderingMesh_interpolation_vertices, &secondaryDeformableObjectRenderingMesh_interpolation_weights);
	
  }
  else
    renderSecondaryDeformableObject = 0;

  if (!((deformableObject == MASSSPRING) && (massSpringSystemSource == CHAIN)))
  {
    // read the fixed vertices
    // 1-indexed notation
    if (strcmp(fixedVerticesFilename, "__none") == 0)
    {
      numFixedVertices = 0;
      fixedVertices = nullptr;
    }
    else
    {
      if (ListIO::load(fixedVerticesFilename, &numFixedVertices,&fixedVertices) != 0)
      {
        printf("Error reading fixed vertices.\n");
        exit(1);
      }
      ListIO::sort(numFixedVertices, fixedVertices);
    }
  }
  else
  {
    numFixedVertices = 1;
    fixedVertices = (int*) malloc (sizeof(int) * numFixedVertices);
    fixedVertices[0] = massSpringSystem->GetNumParticles();
  }

  if (ListIO::load(fixedVerticesKVFFileName, &numFixedKVFNumber, &fixedKVFVertices) != 0)
  {
	  printf("Error reading fixed vertices.\n");
	  exit(1);
  }

  for (auto i = 0; i < numFixedKVFNumber; i++)
  {
	  KVFVertices.push_back(fixedKVFVertices[i]);
  }
  //存储判断体素形变顶点索引
 // volumetricMesh->SaveObjectVertexsintElement(KVFVertices, secondaryDeformableObjectRenderingMesh->Getn(), secondaryDeformableObjectRenderingMesh_interpolation_numElementVertices, secondaryDeformableObjectRenderingMesh_interpolation_vertices, ObjectVertexIndexInElement);

  printf("Loaded %d fixed vertices. They are:\n",numFixedVertices);
  ListIO::print(numFixedVertices,fixedVertices);
  // create 0-indexed fixed DOFs
  //每个顶点三个方向的DOF
  int numFixedDOFs = 3 * numFixedVertices;
  int * fixedDOFs = (int*) malloc (sizeof(int) * numFixedDOFs);
  //将固定的某个顶点的三维全部选取出来
  for(int i=0; i<numFixedVertices; i++)
  {
    fixedDOFs[3*i+0] = 3*fixedVertices[i]-3;
    fixedDOFs[3*i+1] = 3*fixedVertices[i]-2;
    fixedDOFs[3*i+2] = 3*fixedVertices[i]-1;
  }
  for(int i=0; i<numFixedVertices; i++)
    fixedVertices[i]--;
  printf("Boundary vertices processed.\n");

  // make room for deformation and force vectors
  u = (double*) calloc (3*n, sizeof(double));
  uvel = (double*) calloc (3*n, sizeof(double));
  uaccel = (double*) calloc (3*n, sizeof(double));
  f_ext = (double*) calloc (3*n, sizeof(double));
  f_extBase = (double*) calloc (3*n, sizeof(double));

  for (int i = 0; i < n; i++)
  {
	  f_extBase[3 * i + 0] = allVerticesBaseForce;
	  //f_extBase[3 * i + 1] = allVerticesBaseForce;
	  //f_extBase[3 * i + 2] = allVerticesBaseForce;
  }

  deltau = (double*)calloc(3 * n, sizeof(double));
  preu= (double*)calloc(3 * n, sizeof(double));

  StemExtraForces.resize(1800000, 0);

  //if (ExternFileLineForceDirectory != "none")
  //{
  //    std::vector<std::pair<int, int>> tempLineConfig = GetLineForceConfigurate(ExternFileLineForceDirectory);
  //    LineExtraForces = GenerateLineForce(Common::SamplingFrameNumber, tempLineConfig);
  //    for (int i = 0; i < LineExtraForces.size(); i++)
  //    {
  //        StemExtraForces[i] += LineExtraForces[i];
  //    }
  //}
  std::vector<std::pair<int, int>> tempLineConfig;




  std::vector<std::vector<double>> tempConfig = GetForceConfigurate(outputFilename, ExternFileDirectory,Theta,Phi, tempLineConfig);
  if (tempLineConfig.size() != 0)
  {
      LineExtraForces = GenerateLineForce(Common::SamplingFrameNumber, tempLineConfig);
      for (int i = 0; i < LineExtraForces.size(); i++)
      {
          StemExtraForces[i] += LineExtraForces[i];
      }
  }
  std::pair<std::vector<int>, std::vector<float>>tempStreePoints;
  std::vector<std::pair<int, int>>tempDeviationDirection;
  std::vector<float> tempFrequency;
  std::vector<float> tempPhase;
  int stemPullvertexNums;
  GetStemAndLeafStreePointsConfigurate(ExternStreePointsDirectory, stemPullvertexNums, tempStreePoints, tempDeviationDirection,tempFrequency, tempPhase);

  pullVertexInfo.StemPullVertexNum = stemPullvertexNums;
  pullVertexInfo.PullVertexIndex = tempStreePoints.first;
  
  pullVertexInfo.LeafExtraForces = LeafExtraForces;
  pullVertexInfo.Scale = tempStreePoints.second;
  pullVertexInfo.DeviationDirection = tempDeviationDirection;
  //1000
  //5828, 6539, 8117
  //6172,6552,2768
  //6270,5852
  //std::vector<int>pullVertexIndex = { 6172,6552,2768 };
  //std::vector<int>pullVertexIndex = { 6270,5852 ,6821,2910,7408,11070,8341,10950,232,10393,11621,10222,3000,10682 };
  //std::vector<float>scale = { 1.0,0.8,0.08,0.05,-0.02,-0.05 };
  //600
  //std::vector<float>scale = { 1.0,0.8,0.05,0.03,-0.009,-0.03 };
  //3000
  //std::vector<float>scale = { 1.0,1.0,0.1,0.1,0.1,0.1 };
  //std::vector<float>scale = { 1.0,1.0,1.0,0.01,-0.009,-0.009 };
 

  for (int i = 0; i < tempConfig.size(); i++)
  {
	  std::vector<int> tempStemForces= GenerateSamplingForce(18000, tempConfig[i][0], tempConfig[i][1], tempConfig[i][2], tempConfig[i][3], 600);
	  for (int k = 0; k < tempStemForces.size(); k++)
	  {
          //std::cout << tempConfig[i].size() << std::endl;
          if (tempConfig[i].size() ==5 )
          {
              if (tempStemForces[k] < tempConfig[i][3])
              {
                  tempStemForces[k] = tempConfig[i][3];                
              }
             
          }
          if (tempConfig[i].size() == 6)
          {
              if (tempStemForces[k] < tempConfig[i][3])
              {
                  tempStemForces[k] = tempConfig[i][3];          
                  tempStemForces[k] -= tempConfig[i][5];
                
              }

          }
		  StemExtraForces[k] += tempStemForces[k];
	  }
  }
  pullVertexInfo.StemExtraForces = StemExtraForces;
  for (int k = pullVertexInfo.StemPullVertexNum; k < tempFrequency.size() ; k++)
  {
      std::vector<int> tempLeafExtraForces;
      tempLeafExtraForces.resize(18000, 0);
      for (int i = 0; i < tempConfig.size(); i++)
      {
          std::vector<int> tempLeafForces = GenerateSamplingForce(0, tempConfig[i][0], tempConfig[i][1] * tempFrequency[k], tempConfig[i][2]+ tempPhase[k], tempConfig[i][3], 600);
          for (int k = 0; k < tempLeafForces.size(); k++)
          {
              std::cout << tempConfig[i].size() << std::endl;
              if (tempConfig[i].size() == 5)
              {
                  if (tempLeafForces[k] < tempConfig[i][3])
                  {
                      tempLeafForces[k] = tempConfig[i][3];
                  }

              }
              if (tempConfig[i].size() == 6)
              {
                  if (tempLeafForces[k] < tempConfig[i][3])
                  { 
                      tempLeafForces[k] = tempConfig[i][3];   
                      tempLeafForces[k] -= tempConfig[i][5];
                  }

              }
              tempLeafExtraForces[k] += tempLeafForces[k];
          }
      }
      pullVertexInfo.LeafExtraForces.push_back(tempLeafExtraForces);
  }


  ///////////////////////////
  //StemExtraForces = GenerateSamplingForce(180, tempConfig[0], tempConfig[1], tempConfig[2], tempConfig[3], 6);
  //LeafExtraForces = GenerateSamplingForce(180, tempConfig[0], tempConfig[1], tempConfig[2], tempConfig[3], 6);
  


  for (int i = 0; i < pullVertexInfo.StemPullVertexNum; i++)
  {
	  constantpulledVertex[i] = pullVertexInfo.PullVertexIndex[i];
  }
  

  // load initial condition
  if (strcmp(initialPositionFilename, "__none") != 0)
  {
    int m1, n1;
    ReadMatrixFromDisk_(initialPositionFilename, &m1, &n1, &uInitial);
    if ((m1 != 3*n) || (n1 != 1))
    {
      printf("Error: initial position matrix size mismatch.\n");
      exit(1);
    }
  }
  else if ((deformableObject == MASSSPRING) && (massSpringSystemSource == CHAIN))
  {
    uInitial = (double*) calloc (3*n, sizeof(double));
    int numParticles = massSpringSystem->GetNumParticles(); 
    for(int i=0; i<numParticles; i++)
    {
      uInitial[3*i+0] = 1.0 - ((numParticles == 1) ? 1.0 : 1.0 * i / (numParticles - 1));
      uInitial[3*i+1] = 1.0 - ((numParticles == 1) ? 0.0 : 1.0 * i / (numParticles-1));
      uInitial[3*i+2] = 0.0;
    }
  }
  else
    uInitial = (double*) calloc (3*n, sizeof(double));

  // load initial velocity
  if (strcmp(initialVelocityFilename, "__none") != 0)
  {
    int m1, n1;
    ReadMatrixFromDisk_(initialVelocityFilename, &m1, &n1, &velInitial);
    if ((m1 != 3*n) || (n1 != 1))
    {
      printf("Error: initial position matrix size mismatch.\n");
      exit(1);
    }
  }

  // load force loads
  if (strcmp(forceLoadsFilename, "__none") != 0)
  {
    int m1;
    ReadMatrixFromDisk_(forceLoadsFilename, &m1, &numForceLoads, &forceLoads);
    if (m1 != 3*n)
    {
      printf("Mismatch in the dimension of the force load matrix.\n");
      exit(1);
    }
  }

  // create force model, to be used by the integrator
  printf("Creating force model...\n");
  if (deformableObject == STVK)
  {
    printf("Force model: STVK\n");
    fflush(stdout);

    stVKStencilForceModel = new StVKStencilForceModel(stVKFEM);
    stencilForceModel = stVKStencilForceModel;
  }

  if (deformableObject == COROTLINFEM)
  {
    printf("Force model: COROTLINFEM\n");

	//求解K矩阵
    CorotationalLinearFEM * corotationalLinearFEM = new CorotationalLinearFEM(volumetricMesh);

    corotationalLinearFEMStencilForceModel = new CorotationalLinearFEMStencilForceModel(corotationalLinearFEM);
    corotationalLinearFEMStencilForceModel->SetWarp(corotationalLinearFEM_warp);

    stencilForceModel = corotationalLinearFEMStencilForceModel;
  }

  if (deformableObject == LINFEM)
  {
    printf("Force model: LINFEM\n");

    //assert(stVKStencilForceModel != nullptr);
    linearFEMStencilForceModel = new LinearFEMStencilForceModel(stVKStencilForceModel);
    stencilForceModel = linearFEMStencilForceModel;
  }

  if (deformableObject == INVERTIBLEFEM)
  {
    printf("Force model: INVERTIBLEFEM\n");
    TetMesh * tetMesh = dynamic_cast<TetMesh*>(volumetricMesh);
    if (tetMesh == nullptr)
    {
      printf("Error: the input mesh is not a tet mesh (Invertible FEM deformable model).\n");
      exit(1);
    }

    IsotropicMaterial * isotropicMaterial = nullptr;

    // create the invertible material model
    if (strcmp(invertibleMaterialString, "StVK") == 0)
      invertibleMaterial = INV_STVK;
    if (strcmp(invertibleMaterialString, "neoHookean") == 0)
      invertibleMaterial = INV_NEOHOOKEAN;
    if (strcmp(invertibleMaterialString, "MooneyRivlin") == 0)
      invertibleMaterial = INV_MOONEYRIVLIN;

    switch (invertibleMaterial)
    {
      case INV_STVK:
      {
        isotropicMaterial = new StVKIsotropicMaterial(tetMesh, enableCompressionResistance, compressionResistance);
        printf("Invertible material: StVK.\n");
        break;
      }
	  /******************************/
	  //超弹性材料
      case INV_NEOHOOKEAN:
        isotropicMaterial = new NeoHookeanIsotropicMaterial(tetMesh, enableCompressionResistance, compressionResistance);
        printf("Invertible material: neo-Hookean.\n");
        break;

      case INV_MOONEYRIVLIN:
        isotropicMaterial = new MooneyRivlinIsotropicMaterial(tetMesh, enableCompressionResistance, compressionResistance);
        printf("Invertible material: Mooney-Rivlin.\n");
        break;

      default:
        printf("Error: invalid invertible material type.\n");
        exit(1);
        break;
    }

    // create the invertible FEM deformable model
    IsotropicHyperelasticFEM * isotropicHyperelasticFEM = new IsotropicHyperelasticFEM(tetMesh, isotropicMaterial, inversionThreshold, addGravity, g);
    isotropicHyperelasticFEMStencilForceModel = new IsotropicHyperelasticFEMStencilForceModel(isotropicHyperelasticFEM);
    stencilForceModel = isotropicHyperelasticFEMStencilForceModel;
  }

  if (deformableObject == MASSSPRING)
  {
    printf("Force model: MASSSPRING\n");

    massSpringStencilForceModel = new MassSpringStencilForceModel(massSpringSystem);
    stencilForceModel = massSpringStencilForceModel;

    renderMassSprings = new RenderSprings();
  }

  assert(stencilForceModel != nullptr);
  forceModelAssembler = new ForceModelAssembler(stencilForceModel);
  forceModel = forceModelAssembler;

  // initialize the integrator
  printf("Initializing the integrator, n = %d...\n", n);
  printf("Solver type: %s\n", solverMethod);

  integratorBaseSparse = nullptr;
  if (solver == IMPLICITNEWMARK)
  {
    implicitNewmarkSparse = new ImplicitNewmarkSparse(3*n, timeStep, massMatrix, forceModel, numFixedDOFs, fixedDOFs,
       dampingMassCoef, dampingStiffnessCoef, maxIterations, epsilon, newmarkBeta, newmarkGamma, numSolverThreads);
    integratorBaseSparse = implicitNewmarkSparse;
  }
  else if (solver == IMPLICITBACKWARDEULER)
  {
    implicitNewmarkSparse = new ImplicitBackwardEulerSparse(3*n, timeStep, massMatrix, forceModel, numFixedDOFs, fixedDOFs,
       dampingMassCoef, dampingStiffnessCoef, maxIterations, epsilon, numSolverThreads);
    integratorBaseSparse = implicitNewmarkSparse;
  }
  else if (solver == EULER)
  {
    int symplectic = 0;
    integratorBaseSparse = new EulerSparse(3*n, timeStep, massMatrix, forceModel, symplectic, numFixedDOFs, fixedDOFs, dampingMassCoef);
  }
  else if (solver == SYMPLECTICEULER)
  {
    int symplectic = 1;
    integratorBaseSparse = new EulerSparse(3*n, timeStep, massMatrix, forceModel, symplectic, numFixedDOFs, fixedDOFs, dampingMassCoef);
  }
  else if (solver == CENTRALDIFFERENCES)
  {
    integratorBaseSparse = new CentralDifferencesSparse(3*n, timeStep, massMatrix, forceModel, numFixedDOFs, fixedDOFs, dampingMassCoef, dampingStiffnessCoef, centralDifferencesTangentialDampingUpdateMode, numSolverThreads);
  }

  integratorBase = integratorBaseSparse;

  if (integratorBase == nullptr)
  {
    printf("Error: failed to initialize numerical integrator.\n");
    exit(1);
  }

  // set integration parameters
  integratorBaseSparse->SetDampingMatrix(LaplacianDampingMatrix);
  integratorBase->ResetToRest();
  integratorBase->SetState(uInitial, velInitial);
  integratorBase->SetTimestep(timeStep / substepsPerTimeStep);

  if (implicitNewmarkSparse != nullptr)
  {
    implicitNewmarkSparse->UseStaticSolver(staticSolver);
    if (velInitial != nullptr)
      implicitNewmarkSparse->SetState(implicitNewmarkSparse->Getq(), velInitial);
  }

  // load any external geometry file (e.g. some static scene for decoration; usually there will be none)
  if (strcmp(extraSceneGeometryFilename,"__none") != 0)
  {
    extraSceneGeometry = new SceneObject(extraSceneGeometryFilename);
    extraSceneGeometry->BuildNormals(85.0);
  }
  else
    extraSceneGeometry = nullptr;

  // set up the ground plane (for rendering)
  renderGroundPlane = (strcmp(groundPlaneString, "__none") != 0);
  if (renderGroundPlane)
  {
    double groundPlaneR, groundPlaneG, groundPlaneB;
    double groundPlaneAmbient, groundPlaneDiffuse, groundPlaneSpecular, groundPlaneShininess;
    sscanf(groundPlaneString,"%lf,%lf,%lf,r%lf,g%lf,b%lf,a%lf,d%lf,s%lf,sh%lf", &groundPlaneHeight, &groundPlaneLightHeight, &groundPlaneSize, &groundPlaneR, &groundPlaneG, &groundPlaneB, &groundPlaneAmbient, &groundPlaneDiffuse, &groundPlaneSpecular, &groundPlaneShininess);
    displayListGround = glGenLists(1);
    glNewList(displayListGround, GL_COMPILE);
    RenderGroundPlane(groundPlaneHeight, groundPlaneR, groundPlaneG, groundPlaneB, groundPlaneAmbient, groundPlaneDiffuse, groundPlaneSpecular, groundPlaneShininess);
    glEndList();
  }

  // set background color
  int colorR, colorG, colorB;
  sscanf(backgroundColorString, "%d %d %d", &colorR, &colorG, &colorB);
  glClearColor(1.0 * colorR / 255, 1.0 * colorG / 255, 1.0 * colorB / 255, 0.0);

  Sync_GLUI();
  callAllUICallBacks();

  titleBarCounter.StartCounter();
}

// set up the configuration file
void initConfigurations()
{
  printf("Parsing configuration file %s...\n", configFilename.c_str());
  ConfigFile configFile;

  // specify the entries of the config file

  // at least one of the following must be present:
  configFile.addOptionOptional("volumetricMeshFilename", volumetricMeshFilename, "__none");
  configFile.addOptionOptional("customMassSpringSystem", customMassSpringSystem, "__none");
  configFile.addOptionOptional("deformableObjectMethod", deformableObjectMethod, "StVK");
  configFile.addOptionOptional("massSpringSystemObjConfigFilename", massSpringSystemObjConfigFilename, "__none");
  configFile.addOptionOptional("massSpringSystemTetMeshConfigFilename", massSpringSystemTetMeshConfigFilename, "__none");
  configFile.addOptionOptional("massSpringSystemCubicMeshConfigFilename", massSpringSystemCubicMeshConfigFilename, "__none");

  // option for corotational linear FEM: if warp is disabled, one gets purely linear FEM
  configFile.addOptionOptional("corotationalLinearFEM_warp", &corotationalLinearFEM_warp, corotationalLinearFEM_warp);

  configFile.addOptionOptional("implicitSolverMethod", implicitSolverMethod, "none"); // this is now obsolete, but preserved for backward compatibility, use "solver" below
  configFile.addOptionOptional("solver", solverMethod, "implicitNewmark");

  configFile.addOptionOptional("centralDifferencesTangentialDampingUpdateMode", &centralDifferencesTangentialDampingUpdateMode, centralDifferencesTangentialDampingUpdateMode);

  configFile.addOptionOptional("initialPositionFilename", initialPositionFilename, "__none");
  configFile.addOptionOptional("initialVelocityFilename", initialVelocityFilename, "__none");
  configFile.addOptionOptional("outputFilename", outputFilename, "__none");
  configFile.addOptionOptional("addGravity", &addGravity, addGravity);
  configFile.addOptionOptional("g", &g, g);

  configFile.addOptionOptional("renderingMeshFilename", renderingMeshFilename, "__none");
  configFile.addOptionOptional("secondaryRenderingMeshFilename", secondaryRenderingMeshFilename, "__none");
  configFile.addOptionOptional("secondaryRenderingMeshInterpolationFilename", secondaryRenderingMeshInterpolationFilename, "__none");
  configFile.addOptionOptional("useRealTimeNormals", &useRealTimeNormals, 0);
  configFile.addOptionOptional("fixedVerticesFilename", fixedVerticesFilename, "__none");

  configFile.addOptionOptional("fixedVerticesKVFFileName", fixedVerticesKVFFileName, "__none");
  configFile.addOptionOptional("ExternFileDirectory", ExternFileDirectory, "_none");
  configFile.addOptionOptional("ExternFileLineForceDirectory", ExternFileLineForceDirectory, "_none");

  configFile.addOptionOptional("ExternStreePointsDirectory", ExternStreePointsDirectory, "_none");
  configFile.addOptionOptional("uDeformationoutputFileName", uDeformationoutputFileName, "__none");
  configFile.addOptionOptional("ObjectVertexIndexInElement", ObjectVertexIndexInElement, "__none");

  configFile.addOptionOptional("enableCompressionResistance", &enableCompressionResistance, enableCompressionResistance);
  configFile.addOptionOptional("compressionResistance", &compressionResistance, compressionResistance);
  configFile.addOption("timestep", &timeStep);
  configFile.addOptionOptional("substepsPerTimeStep", &substepsPerTimeStep, substepsPerTimeStep);
  configFile.addOptionOptional("syncTimestepWithGraphics", &syncTimestepWithGraphics, syncTimestepWithGraphics);
  configFile.addOption("dampingMassCoef", &dampingMassCoef);
  configFile.addOption("dampingStiffnessCoef", &dampingStiffnessCoef);
  configFile.addOptionOptional("dampingLaplacianCoef", &dampingLaplacianCoef, dampingLaplacianCoef);
  configFile.addOptionOptional("newmarkBeta", &newmarkBeta, newmarkBeta);
  configFile.addOptionOptional("newmarkGamma", &newmarkGamma, newmarkGamma);
  configFile.addOption("deformableObjectCompliance", &deformableObjectCompliance);
  configFile.addOption("frequencyScaling", &frequencyScaling);
  configFile.addOptionOptional("forceNeighborhoodSize", &forceNeighborhoodSize, forceNeighborhoodSize);
  configFile.addOptionOptional("maxIterations", &maxIterations, 1);
  configFile.addOptionOptional("epsilon", &epsilon, 1E-6);
  configFile.addOptionOptional("numInternalForceThreads", &numInternalForceThreads, 0);
  configFile.addOptionOptional("numSolverThreads", &numSolverThreads, 1);
  configFile.addOptionOptional("inversionThreshold", &inversionThreshold, -DBL_MAX);
  configFile.addOptionOptional("forceLoadsFilename", forceLoadsFilename, "__none");

  configFile.addOptionOptional("windowWidth", &windowWidth, windowWidth);
  configFile.addOptionOptional("windowHeight", &windowHeight, windowHeight);
  configFile.addOptionOptional("cameraRadius", &cameraRadius, 17.5);
  configFile.addOptionOptional("focusPositionX", &focusPositionX, 0.0);
  configFile.addOptionOptional("focusPositionY", &focusPositionY, 0.0);
  configFile.addOptionOptional("focusPositionZ", &focusPositionZ, 0.0);
  configFile.addOptionOptional("cameraLongitude", &cameraLongitude, -60.0);
  configFile.addOptionOptional("cameraLattitude", &cameraLattitude, 20.0);
  configFile.addOptionOptional("renderWireframe", &renderWireframe, 1);
  configFile.addOptionOptional("renderSecondaryDeformableObject", &renderSecondaryDeformableObject, renderSecondaryDeformableObject);
  configFile.addOptionOptional("renderAxes", &renderAxes, renderAxes);
  configFile.addOptionOptional("extraSceneGeometry", extraSceneGeometryFilename, "__none");
  configFile.addOptionOptional("enableTextures", &enableTextures, enableTextures);
  configFile.addOptionOptional("backgroundColor", backgroundColorString, backgroundColorString);
  configFile.addOption("lightingConfigFilename", lightingConfigFilename);
  configFile.addOptionOptional("groundPlane", groundPlaneString, "__none");

  configFile.addOptionOptional("singleStepMode", &singleStepMode, singleStepMode);
  configFile.addOptionOptional("pauseSimulation", &pauseSimulation, pauseSimulation);
  configFile.addOptionOptional("lockAt30Hz", &lockAt30Hz, lockAt30Hz);

  configFile.addOptionOptional("invertibleMaterial", invertibleMaterialString, invertibleMaterialString);

  // parse the configuration file
  if (configFile.parseOptions((char*)configFilename.c_str()) != 0)
  {
    printf("Error parsing options.\n");
    exit(1);
  }

  // the config variables have now been loaded with their specified values

  // informatively print the variables (with assigned values) that were just parsed
  configFile.printOptions();

  // set the solver based on config file input
  solver = UNKNOWN;
  if (strcmp(implicitSolverMethod, "implicitNewmark") == 0)
    solver = IMPLICITNEWMARK;
  if (strcmp(implicitSolverMethod, "implicitBackwardEuler") == 0)
    solver = IMPLICITBACKWARDEULER;

  if (strcmp(solverMethod, "implicitNewmark") == 0)
    solver = IMPLICITNEWMARK;
  if (strcmp(solverMethod, "implicitBackwardEuler") == 0)
    solver = IMPLICITBACKWARDEULER;
  if (strcmp(solverMethod, "Euler") == 0)
    solver = EULER;
  if (strcmp(solverMethod, "symplecticEuler") == 0)
    solver = SYMPLECTICEULER;
  if (strcmp(solverMethod, "centralDifferences") == 0)
    solver = CENTRALDIFFERENCES;

  if (solver == UNKNOWN)
  {
    printf("Error: unknown implicit solver specified.\n");
    exit(1);
  }
}

// GLUI-related functions
void Sync_GLUI()
{
  glui->sync_live();
}

void deformableObjectCompliance_spinnerCallBack(int code)
{
  if (deformableObjectCompliance < 0)
    deformableObjectCompliance = 0;

  glui->sync_live();
}

void timeStep_spinnerCallBack(int code)
{
  if (timeStep < 0)
    timeStep = 0;

  integratorBase->SetTimestep(timeStep / substepsPerTimeStep);

  glui->sync_live();
}

void syncTimestepWithGraphics_checkboxCallBack(int code)
{
  if (syncTimestepWithGraphics)
    timeStep_spinner->disable();
  else
    timeStep_spinner->enable();
}

void frequencyScaling_spinnerCallBack(int code)
{
  if (frequencyScaling < 0)
    frequencyScaling = 0;

  glui->sync_live();

  integratorBase->SetInternalForceScalingFactor(frequencyScaling * frequencyScaling);
}

void newmarkBeta_spinnerCallBack(int code)
{
  if (newmarkBeta < 0)
    newmarkBeta = 0;

  if (newmarkBeta > 0.5)
    newmarkBeta = 0.5;

  if (use1DNewmarkParameterFamily)
  {
    if (newmarkBeta > 0.25)
      newmarkGamma = sqrt(4.0 * newmarkBeta) - 0.5;
    else
      newmarkGamma = 0.5;
  }

  if (implicitNewmarkSparse != nullptr)
  {
    implicitNewmarkSparse->SetNewmarkBeta(newmarkBeta);
    implicitNewmarkSparse->SetNewmarkGamma(newmarkGamma);
  }

  glui->sync_live();
}

void newmarkGamma_spinnerCallBack(int code)
{
  if (newmarkGamma < 0.5)
    newmarkGamma = 0.5;

  if (newmarkGamma > 1.0)
    newmarkGamma = 1.0;

  if (use1DNewmarkParameterFamily)
    newmarkBeta = (newmarkGamma + 0.5) * (newmarkGamma + 0.5) / 4.0;

  if (implicitNewmarkSparse != nullptr)
  {
    implicitNewmarkSparse->SetNewmarkBeta(newmarkBeta);
    implicitNewmarkSparse->SetNewmarkGamma(newmarkGamma);
  }

  glui->sync_live();
}

void newmark_checkboxuse1DNewmarkParameterFamilyCallBack(int code)
{
  if (use1DNewmarkParameterFamily)
  {
    newmarkBeta = (newmarkGamma + 0.5) * (newmarkGamma + 0.5) / 4.0;

    if (implicitNewmarkSparse != nullptr)
    {
      implicitNewmarkSparse->SetNewmarkBeta(newmarkBeta);
      implicitNewmarkSparse->SetNewmarkGamma(newmarkGamma);
    }
  }
  glui->sync_live();
}

void rayleighMass_spinnerCallBack(int code)
{
  if (dampingMassCoef < 0)
    dampingMassCoef = 0;

  integratorBase->SetDampingMassCoef(dampingMassCoef);

  glui->sync_live();
}

void rayleighStiffness_spinnerCallBack(int code)
{
  if (dampingStiffnessCoef < 0)
    dampingStiffnessCoef = 0;

  integratorBase->SetDampingStiffnessCoef(dampingStiffnessCoef);

  glui->sync_live();
}

void laplacianDamping_spinnerCallBack(int code)
{
  glui->sync_live();
}

void timeStepSubdivisions_spinnerCallBack(int code)
{
  if (substepsPerTimeStep < 1)
    substepsPerTimeStep = 1;

  integratorBase->SetTimestep(timeStep / substepsPerTimeStep); 

  glui->sync_live();
}

void stopDeformations_buttonCallBack(int code)
{
  integratorBase->ResetToRest();
  integratorBase->SetState(uInitial);
  memcpy(u, integratorBase->Getq(), sizeof(double) * 3 * n);
  deformableObjectRenderingMesh->SetVertexDeformations(u);
  timestepCounter = 0;
  subTimestepCounter = 0;
}

void staticSolver_checkboxCallBack(int code)
{
  implicitNewmarkSparse->UseStaticSolver(staticSolver);
}

void exit_buttonCallBack(int code)
{
  exit(0);
}

// calls the GLUI callbacks
void callAllUICallBacks()
{
  deformableObjectCompliance_spinnerCallBack(0);
  frequencyScaling_spinnerCallBack(0);
  timeStep_spinnerCallBack(0);
  syncTimestepWithGraphics_checkboxCallBack(0);
  rayleighMass_spinnerCallBack(0);
  rayleighStiffness_spinnerCallBack(0);
  laplacianDamping_spinnerCallBack(0);
  timeStepSubdivisions_spinnerCallBack(0);
  newmarkBeta_spinnerCallBack(0);
  newmarkGamma_spinnerCallBack(0);
  newmark_checkboxuse1DNewmarkParameterFamilyCallBack(0);
}

void initGLUI()
{
  // generate the UI, via the GLUI library

  glui = GLUI_Master.create_glui("Controls", 0, windowWidth + 52, 0);

  glui->add_spinner("Deformable object compliance:", 
     GLUI_SPINNER_FLOAT, &deformableObjectCompliance, 0, 
     deformableObjectCompliance_spinnerCallBack );
  glui->add_spinner("Force kernel size:", GLUI_SPINNER_INT, &forceNeighborhoodSize);

  glui->add_spinner("Frequency scaling:", 
     GLUI_SPINNER_FLOAT, &frequencyScaling, 0, frequencyScaling_spinnerCallBack );

  // ******** newmark beta, gamma *********

  if ((solver == IMPLICITNEWMARK) || (solver == IMPLICITBACKWARDEULER))
  {
    GLUI_Panel * newmark_panel =
       glui->add_panel("Newmark integrator parameters", GLUI_PANEL_EMBOSSED);
    newmark_panel->set_alignment(GLUI_ALIGN_LEFT);

    glui->add_checkbox_to_panel(newmark_panel, "Link Beta and Gamma", 
       &use1DNewmarkParameterFamily, 0, newmark_checkboxuse1DNewmarkParameterFamilyCallBack);

    GLUI_Spinner * newmarkBeta_spinner = 
        glui->add_spinner_to_panel(newmark_panel,"Beta", GLUI_SPINNER_FLOAT,
            &newmarkBeta, 0, newmarkBeta_spinnerCallBack);
    newmarkBeta_spinner->set_speed(0.1);

    GLUI_Spinner * newmarkGamma_spinner = 
        glui->add_spinner_to_panel(newmark_panel,"Gamma", GLUI_SPINNER_FLOAT,
            &newmarkGamma, 0, newmarkGamma_spinnerCallBack);
    newmarkGamma_spinner->set_speed(0.1);

    glui->add_checkbox_to_panel(newmark_panel,"Static solver only", &staticSolver, 0,
       staticSolver_checkboxCallBack);

  }

  // ******** damping ********

  GLUI_Panel * damping_panel =
     glui->add_panel("Damping", GLUI_PANEL_EMBOSSED);
  damping_panel->set_alignment(GLUI_ALIGN_LEFT);

  glui->add_spinner_to_panel(damping_panel,"Mass-proportional", GLUI_SPINNER_FLOAT,
     &dampingMassCoef, 0, rayleighMass_spinnerCallBack);

  glui->add_spinner_to_panel(damping_panel,"Stiffness-proportional", GLUI_SPINNER_FLOAT,
     &dampingStiffnessCoef, 0, rayleighStiffness_spinnerCallBack);

  glui->add_button("Stop deformations", 0, stopDeformations_buttonCallBack);

  // ******** timestep control ********

  //glui->add_separator();

  GLUI_Panel * timeStep_panel =
     glui->add_panel("Timestep control", GLUI_PANEL_EMBOSSED);
  timeStep_panel->set_alignment(GLUI_ALIGN_LEFT);

  glui->add_checkbox_to_panel(timeStep_panel, "Sync with graphics", 
     &syncTimestepWithGraphics, 0, syncTimestepWithGraphics_checkboxCallBack);

  timeStep_spinner = 
     glui->add_spinner_to_panel(timeStep_panel,"Timestep [sec]", GLUI_SPINNER_FLOAT, &timeStep, 0, timeStep_spinnerCallBack);
  timeStep_spinner->set_alignment(GLUI_ALIGN_LEFT);

  if (syncTimestepWithGraphics)
    timeStep_spinner->disable();

  glui->add_spinner_to_panel(timeStep_panel,"Substeps per timestep", GLUI_SPINNER_INT, &substepsPerTimeStep, 0, timeStepSubdivisions_spinnerCallBack);

  glui->add_separator();

  // ******** information ********

  GLUI_Panel * instructions_panel =
     glui->add_panel("Mouse buttons:", GLUI_PANEL_EMBOSSED);
  instructions_panel->set_alignment(GLUI_ALIGN_LEFT);
  glui->add_statictext_to_panel(instructions_panel, "Left + drag: apply force");
  glui->add_statictext_to_panel(instructions_panel, "Middle + drag: zoom in/out");
  glui->add_statictext_to_panel(instructions_panel, "Right + drag: rotate camera");
  glui->add_statictext_to_panel(instructions_panel, "Keys 'e', 'E', 'w': toggle mesh display");

  glui->add_separator();

  GLUI_Panel * performance_panel = glui->add_panel("Performance [sec]:", GLUI_PANEL_EMBOSSED);
  performance_panel->set_alignment(GLUI_ALIGN_LEFT);
  forceAssemblyStaticText = glui->add_statictext_to_panel(performance_panel, "Force assembly: ------------- ");
  systemSolveStaticText = glui->add_statictext_to_panel(performance_panel, "System solve: ------------- ");

  glui->add_separator();

  glui->add_statictext("J. Barbic, F. S. Sin, D. Schroeder");
  glui->add_statictext("CMU, MIT, USC, 2005-2013");

  glui->add_separator();

  glui->add_button("Exit program", 0, exit_buttonCallBack);

  Sync_GLUI();

  glui->set_main_gfx_window( windowID );
}

// main function
int main(int argc, char* argv[])
{
 /* int numFixedArgs = 2;
  if ( argc < numFixedArgs ) 
  {
    printf("Real-time deformable object simulator.\n");
    printf("Usage: %s [config file]\n", argv[0]);
    return 1;
  }*/

  // parse command line options
  //后面第二个配置参数
 /* char * configFilenameC = argv[1];
  opt_t opttable[] =
  {
    { nullptr, 0, nullptr }
  };

  argv += (numFixedArgs-1);
  argc -= (numFixedArgs-1);
  int optup = getopts(argc,argv,opttable);
  if (optup != argc)
  {
    printf("Error parsing options. Error at option %s.\n",argv[optup]);
  }*/

  printf("Starting application.\n");

  //configFilename = string("D:/GraduationProject/Vega/examples/simpleBridge_vox/simpleBridge_vox.config");
 /* configFilename = string("D:/GraduationProject/Vega/models/newgrass/voxelizegrass/voxelizegrass.config");*/
  //configFilename = string("../../models/yellow_tree/tree.config");
  //configFilename = string("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree.config");
  configFilename = string("D:/GraduationProject/New-LargeScaleForest/LargeScaleForest/models/yellow_tree/tree.config");
  printf("Loading scene configuration from %s.\n", configFilename.c_str());

  initConfigurations(); // parse the config file同时输出到cmd
  //strcpy(outputFilename, "D:/GraduationProject/Vega/models/newgrass/voxelizegrass/force/fo");
  //strcpy(forceLoadsFilename, "D:/GraduationProject/Vega/models/newgrass/voxelizegrass/force/fo.f.0031");
  initGLUT(argc, argv, windowTitleBase , windowWidth, windowHeight, &windowID);
  initGraphics(windowWidth, windowHeight); // more OpenGL initialization calls
  initGLUI(); // init the UI
  initSimulation(); // init the simulation
  glutMainLoop(); // you have reached the point of no return..

  return 0;
}

