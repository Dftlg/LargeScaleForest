/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "integrator" library , Copyright (C) 2007 CMU, 2009 MIT, 2018 USC     *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Jernej Barbic                                            *
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
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the BSD-style license that is            *
 * included with this library in the file LICENSE.txt                    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the file     *
 * LICENSE.TXT for more details.                                         *
 *                                                                       *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrixIO.h"
#include "performanceCounter.h"
#include "constrainedDOFs.h"
#include "implicitBackwardEulerSparse.h"

#pragma optimize("", off)

ImplicitBackwardEulerSparse::ImplicitBackwardEulerSparse(int r, double timestep, SparseMatrix * massMatrix_, ForceModel * forceModel_, int numConstrainedDOFs_, int * constrainedDOFs_, double dampingMassCoef, double dampingStiffnessCoef, int maxIterations, double epsilon, int numSolverThreads_): ImplicitNewmarkSparse(r, timestep, massMatrix_, forceModel_, numConstrainedDOFs_, constrainedDOFs_, dampingMassCoef, dampingStiffnessCoef, maxIterations, epsilon, 0.25, 0.5, numSolverThreads_)
{
}

ImplicitBackwardEulerSparse::~ImplicitBackwardEulerSparse()
{
}

// sets the state based on given q, qvel
// automatically computes acceleration assuming zero external force
int ImplicitBackwardEulerSparse::SetState(double * q_, double * qvel_)
{
  memcpy(q, q_, sizeof(double)*r);

  if (qvel_ != NULL)
    memcpy(qvel, qvel_, sizeof(double)*r);

  return 0;
}

int ImplicitBackwardEulerSparse::DoTimestep()
{
  int numIter = 0;

  double error0 = 0; // error after the first step
  double errorQuotient;

  // store current amplitudes振幅 and set initial guesses for qaccel, qvel加速度和速度 q是形变振幅
  for(int i=0; i<r; i++)
  {
    qaccel_1[i] = qaccel[i] = 0; // acceleration is actually not used in this integrator
    q_1[i] = q[i]; 
    qvel_1[i] = qvel[i];
  }

  do
  {
    PerformanceCounter counterForceAssemblyTime;
	//在这里临时算的internalforce和tangentstiffness
	//调用到forcessModelAssember.cpp获取
    forceModel->GetForceAndMatrix(q, internalForces, tangentStiffnessMatrix);
    counterForceAssemblyTime.StopCounter();
    forceAssemblyTime = counterForceAssemblyTime.GetElapsedTime();

	temptangentStiffnessMatrix = tangentStiffnessMatrix;
	//tangentStiffnessMatrix->Save("D:\\GraduationProject\\LargeScaleForest\\models\\8.10\\test\\K2.txt");
    //tangentStiffnessMatrix->Print();
    //tangentStiffnessMatrix->Save("K");

    // scale internal forces
    for(int i=0; i<r; i++)
      internalForces[i] *= internalForceScalingFactor;

/*
    printf("q:\n");
    for(int i=0; i<r; i++)
      printf("%G ", q[i]);
    printf("\n");

    printf("Internal forces:\n");
    for(int i=0; i<r; i++)
      printf("%G ", internalForces[i]);
    printf("\n");
*/

    // scale stiffness matrix 应该是K矩阵
    *tangentStiffnessMatrix *= internalForceScalingFactor;

    memset(qresidual, 0, sizeof(double) * r);

    if (useStaticSolver)
    {
		//这个qdelta是什么
      // fint + K * qdelta = fext

      // add externalForces, internalForces
      for(int i=0; i<r; i++)
      {
        qresidual[i] = externalForces[i] - internalForces[i];
        qdelta[i] = qresidual[i];
      }
    }
    else
    {
      // compute D_Rayleigh = dampingStiffnessCoef * tangentStiffnessMatrix + dampingMassCoef * massMatrix
		//计算瑞利阻尼dampingMassCoef瑞利阻尼a值,dampingstiffnessCoef瑞利阻尼β值。
		//tangentStiffnessMatrix为切线刚度矩阵K
      tangentStiffnessMatrix->ScalarMultiply(dampingStiffnessCoef, rayleighDampingMatrix);
      rayleighDampingMatrix->AddSubMatrix(dampingMassCoef, *massMatrix);

      if (tangentStiffnessMatrixOffset != NULL)
        tangentStiffnessMatrix->AddSubMatrix(1.0, *tangentStiffnessMatrixOffset, 2);

      // build effective stiffness: 
      // Keff = M + h D + h^2 * K
      // compute force residual, store it into aux variable qresidual 残差
	  //▲t(fext-fint(ut)-(▲t K+D)vt)
      // qresidual = h * (-D qdot - fint + fext - h * K * qdot)) // this is semi-implicit Euler
      // qresidual = M (qvel_1 - qvel) + h * (-D qdot - fint + fext - K * (q_1 - q + h qdot) )) // for fully implicit Euler

      if (numIter != 0) // can skip on first iteration (zero contribution)
      {
		 //K*▲u
        // add K * (q_1 - q) to qresidual (will multiply by -h later)
        for(int i=0; i<r; i++)
          buffer[i] = q_1[i] - q[i];
        tangentStiffnessMatrix->MultiplyVectorAdd(buffer, qresidual);
      }

      //add mass matrix and damping matrix to tangentStiffnessMatrix
	  //用来计算A这个System Matrix矩阵
      *tangentStiffnessMatrix *= timestep; //K= t * K
      *tangentStiffnessMatrix += *rayleighDampingMatrix; //K=K+D_RayLeigh= t * K + D_Rayleigh
      tangentStiffnessMatrix->AddSubMatrix(1.0, *dampingMatrix, 1); // at this point, tangentStiffnessMatrix = t * K + (D_Rayleigh + D_exteral)

	  // K=K+dampingMatrix=t*K+D_Rayleigh + D_dampMatrix=  t*k+C 

	  //qresidual = K * v = (t*K + C)v  此时qresidual为(t*K+C)v
      tangentStiffnessMatrix->MultiplyVectorAdd(qvel, qresidual);//(t*K+D)v  

	  //K=t(t*K+C)
      *tangentStiffnessMatrix *= timestep; // h^2 * K + h * (D_Rayleigh + D_externnal)
	  //K=t(t*K+C)+M=t^2K+tC+M  此时的tangentStiffnessMatrix为A矩阵
      tangentStiffnessMatrix->AddSubMatrix(1.0, *massMatrix); // h^2 * K + h * (D_Rayleigh + D_external) + M


	  //tangentStiffnessMatrix->Save("D:\\GraduationProject\\LargeScaleForest\\models\\8.10\\test\\K3.txt");
      // add externalForces, internalForces
	  // qresidual=qresidual + Fint-Fext= (t*K+C)v+Fint-Fext
	  //qresidual=-t[(t*K+C)v+Fint-Fext]=t[-(t*K+C)v-Fint+Fext] 此时与隐式欧拉residual步骤相同
      for(int i=0; i<r; i++)
      {
        qresidual[i] += internalForces[i] - externalForces[i];
        qresidual[i] *= -timestep;
      }

      if (numIter != 0) // can skip on first iteration (zero contribution)
      {
        // add M * (qvel_1 - qvel) to qresidual
        for(int i=0; i<r; i++)
          buffer[i] = qvel_1[i] - qvel[i];
        massMatrix->MultiplyVectorAdd(buffer, qresidual);
      }

      for(int i=0; i<r; i++)
        qdelta[i] = qresidual[i];
    }

/*
    printf("internal forces:\n");
    for(int i=0; i<r; i++)
      printf("%G ", internalForces[i]);
    printf("\n");

    printf("external forces:\n");
    for(int i=0; i<r; i++)
      printf("%G ", externalForces[i]);
    printf("\n");

    printf("residual:\n");
    for(int i=0; i<r; i++)
      printf("%G ", -qresidual[i]);
    printf("\n");
*/

    //tangentStiffnessMatrix->Save("Keff");

    // remove rows corresponding to fixed vertices from qdelta, and store the result in bufferConstrained
    ConstrainedDOFs::RemoveDOFs(r, bufferConstrained, qdelta, numConstrainedDOFs, constrainedDOFs);

    double error = 0.0;
    for(int i=0; i<r - numConstrainedDOFs; i++)
      error += bufferConstrained[i] * bufferConstrained[i];

    //printf("numIter: %d error2: %G\n", numIter, error);

    // on the first iteration, compute initial error
    if (numIter == 0) 
    {
      error0 = error;
      errorQuotient = 1.0;
    }
    else
    {
      // error divided by the initial error, before performing this iteration
      errorQuotient = error / error0; 
    }

    if (errorQuotient < epsilon * epsilon)
      break;

    systemMatrix->AssignSuperMatrix(*tangentStiffnessMatrix);
	//systemMatrix->Save("D:\\GraduationProject\\LargeScaleForest\\models\\8.10\\test\\K3.txt");
    // solve: systemMatrix * buffer = bufferConstrained

    PerformanceCounter counterSystemSolveTime;
    memset(buffer, 0, sizeof(double) * r);

    #ifdef SPOOLES
      int info;
      if (numSolverThreads > 1)
      {
        SPOOLESSolverMT * solver = new SPOOLESSolverMT(systemMatrix, numSolverThreads);
        info = solver->SolveLinearSystem(buffer, bufferConstrained);
        delete(solver);
      }
      else
      {
        SPOOLESSolver * solver = new SPOOLESSolver(systemMatrix);
        info = solver->SolveLinearSystem(buffer, bufferConstrained);
        delete(solver);
      }
      char solverString[16] = "SPOOLES";
    #endif

    #ifdef PARDISO
      int info = pardisoSolver->FactorMatrix(systemMatrix);
	  // A(\dot q)=residual
      if (info == 0)
        info = pardisoSolver->SolveLinearSystem(buffer, bufferConstrained);
      char solverString[16] = "PARDISO";
    #endif

    #ifdef PCG
      int info = jacobiPreconditionedCGSolver->SolveLinearSystemWithJacobiPreconditioner(buffer, bufferConstrained, 1e-6, 10000);
      if (info > 0)
        info = 0;
      char solverString[16] = "PCG";
    #endif

    if (info != 0)
    {
      printf("Error: %s sparse solver returned non-zero exit status %d.\n", solverString, (int)info);
      return 1;
    }

    counterSystemSolveTime.StopCounter();
    systemSolveTime = counterSystemSolveTime.GetElapsedTime();

    ConstrainedDOFs::InsertDOFs(r, buffer, qdelta, numConstrainedDOFs, constrainedDOFs);

/*
    printf("qdelta:\n");
    for(int i=0; i<r; i++)
      printf("%G ", qdelta[i]);
    printf("\n");
    exit(1);
*/
    // update state
    if (useStaticSolver)
    {
      for(int i=0; i<r; i++)
      {
        q[i] += qdelta[i];
        qvel[i] = (q[i] - q_1[i]) / timestep;
      }
    }
    else
    {
      for(int i=0; i<r; i++)
      {
        qvel[i] += qdelta[i];
        q[i] += q_1[i] - q[i] + timestep * qvel[i];
      }
    }

    for(int i=0; i<numConstrainedDOFs; i++)
      q[constrainedDOFs[i]] = qvel[constrainedDOFs[i]] = qaccel[constrainedDOFs[i]] = 0.0;

    numIter++;
  }
  while (numIter < maxIterations);

/*
  printf("q:\n");
  for(int i=0; i<r; i++)
    printf("%G ", q[i]);
  printf("\n");

  printf("qvel:\n");
  for(int i=0; i<r; i++)
    printf("%G ", qvel[i]);
  printf("\n");
*/

  //printf("Num iterations performed: %d\n",numIter);
  //if ((numIter >= maxIterations) && (maxIterations > 1))
  //{
    //printf("Warning: method did not converge in max number of iterations.\n");
  //}

  return 0;
}

void ImplicitBackwardEulerSparse::WriteKRFextVMartixToFile(const std::string & vFilePath, int vFrameIndex)
{
	const size_t last_slash_idx = vFilePath.rfind('.txt');
	std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + ".kvf";

	std::ofstream connectionFile;
	connectionFile.open(FramesBlockFileName, std::ios::in | std::ios::app);

	if (connectionFile.is_open())
	{
		/*int * temp= temptangentStiffnessMatrix->GetRowLengths();
		for (int i = 0; i < r; i++)
		{
			connectionFile << temp[i] << " " << std::endl;
		}*/


		//输出帧号
		connectionFile << vFrameIndex << std::endl;
		connectionFile << "u" << std::endl;
		for (int i = 0; i < r; i++)
			connectionFile << q[i] << " ";
		connectionFile << std::endl;
		//输出内力
		connectionFile << "internalForces" << std::endl;
		for (int i = 0; i < r; i++)
			connectionFile << internalForces[i] << " ";
		connectionFile << std::endl;
		connectionFile << "Kmatrix" << std::endl;
		//输出刚度矩阵或者输出A矩阵
		int temRows = temptangentStiffnessMatrix->GetNumRows();

		for (int i = 0; i < temptangentStiffnessMatrix->GetNumRows(); i++)
		{
			for (int j = 0; j < temptangentStiffnessMatrix->GetRowLength(i); j++)
			{
				//int temRowLength = temptangentStiffnessMatrix->GetRowLength(i);
				int index = temptangentStiffnessMatrix->GetColumnIndex(i, j);
				double entry = temptangentStiffnessMatrix->GetEntry(i, j);
				//connectionFile << i << " " << index << " " << entry << "\n";
				connectionFile << entry << " ";
			}
			connectionFile << "\n";
		}
			
		//输出速度
		connectionFile << "velocity" << std::endl;
		for (int i = 0; i < r; i++)
			connectionFile << qvel[i] << " ";
		connectionFile << std::endl;
	}
	

}

void ImplicitBackwardEulerSparse::WriteSpecificKRFextVMattixToFile(const std::string &vFilePath, int vFrameIndex, std::vector<int>& vElementIndex, std::vector<int> & vForce)
{
    const size_t last_slash_idx = vFilePath.rfind('.txt');
    std::string FramesBlockFileName = vFilePath.substr(0, last_slash_idx - 3);
    FramesBlockFileName = FramesBlockFileName + ".spkvf";
    std::ofstream connectionFile;
    connectionFile.open(FramesBlockFileName, std::ios::in | std::ios::app);

    int ElementNumber = forceModel->GetStencilForceModel()->GetNumStencilVertices(0);

    if (connectionFile.is_open())
    {
        //输出体素个数
        connectionFile << "ElementSize" << std::endl;
        connectionFile << vElementIndex.size() << std::endl;
        //输出帧号
        connectionFile << "FrameIndex" << std::endl;
        connectionFile << vFrameIndex << std::endl;

        connectionFile << "Force" << std::endl;
        for (auto forcenumber = 0; forcenumber < vForce.size(); forcenumber++)
        {
            connectionFile << vForce[forcenumber] << " ";
        }
        connectionFile << "\n";
        connectionFile << "Kmatrix" << std::endl;
        std::map<int, std::vector<double>> vertexInternalForce;
        std::map<int, std::vector<double>> vertexVel;
        std::map<int, std::vector<double>> vertexPos;
        std::vector<int>RowLength;
        std::vector<std::vector<double>>KMatrix;
        for (int i = 0; i < vElementIndex.size(); i++)
        {
            //根据元素的索引号获取元素的八个顶点索引
            const int *vIndices = forceModel->GetStencilForceModel()->GetStencilVertexIndices(0, vElementIndex[i]);
            for (int v = 0; v < ElementNumber; v++)
            {
                //存储当前ele的某个顶点内力值
                if (vertexInternalForce.count(vIndices[v]) == 0)
                {
                    std::vector<double> tempForcesxyz;
                    tempForcesxyz.push_back(internalForces[vIndices[v] * 3]);
                    tempForcesxyz.push_back(internalForces[vIndices[v] * 3 + 1]);
                    tempForcesxyz.push_back(internalForces[vIndices[v] * 3 + 2]);
                    vertexInternalForce.insert(std::make_pair(vIndices[v], tempForcesxyz));

                    std::vector<double> tempVelsxyz;
                    tempVelsxyz.push_back(qvel[vIndices[v] * 3]);
                    tempVelsxyz.push_back(qvel[vIndices[v] * 3 + 1]);
                    tempVelsxyz.push_back(qvel[vIndices[v] * 3 + 2]);
                    vertexVel.insert(std::make_pair(vIndices[v], tempVelsxyz));


                    //存储当前ele的某个顶点相关的某一维度相关顶点维度的数据 KMatrix
                    for (int j = 0; j < 3; j++)
                    {
                        RowLength.push_back(temptangentStiffnessMatrix->GetRowLength(vIndices[v] * 3 + j));
                        std::vector<double>RowMatrix;
                        for (int k = 0; k < RowLength.back(); k++)
                        {
                            RowMatrix.push_back(temptangentStiffnessMatrix->GetEntry(vIndices[v] * 3 + j, k));
                            double entry = temptangentStiffnessMatrix->GetEntry(vIndices[v] * 3 + j, k);
                        }
                        KMatrix.push_back(RowMatrix);
                    }
                }
            }
        }

        connectionFile << RowLength.size() << std::endl;
        for (int i = 0; i < RowLength.size(); i++)
        {
            connectionFile << RowLength[i] << " ";
            for (int k = 0; k < RowLength[i]; k++)
            {
                connectionFile << KMatrix[i][k] << " ";
            }
            connectionFile << "\n";
        }

        connectionFile << "internalForces" << std::endl;
        connectionFile << vertexInternalForce.size() << std::endl;
        for (auto it = vertexInternalForce.begin(); it != vertexInternalForce.end(); ++it)
        {
            connectionFile << (it)->first << " ";
            for (int i = 0; i < 3; i++)
                connectionFile << it->second[i] << " ";
            connectionFile << "\n";
        }

        connectionFile << "velocity" << std::endl;
        connectionFile << vertexVel.size() << std::endl;
        for (auto it = vertexVel.begin(); it != vertexVel.end(); ++it)
        {
            connectionFile << (it)->first << " ";
            for (int i = 0; i < 3; i++)
                connectionFile << it->second[i] << " ";
            connectionFile << "\n";
        }
    }
#pragma endregion
}

#pragma optimize("", on)
