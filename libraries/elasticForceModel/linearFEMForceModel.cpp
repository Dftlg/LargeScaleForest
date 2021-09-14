/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "forceModel" library , Copyright (C) 2007 CMU, 2009 MIT, 2018 USC     *
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

#include "linearFEMForceModel.h"
#include "StVKStiffnessMatrix.h"

LinearFEMForceModel::LinearFEMForceModel(StVKInternalForces * stVKInternalForces) 
{
  StVKStiffnessMatrix * stVKStiffnessMatrix = new StVKStiffnessMatrix(stVKInternalForces);
  stVKStiffnessMatrix->GetStiffnessMatrixTopology(&K);
  r = K->GetNumRows();

  double * zero = (double*) calloc (K->GetNumRows(), sizeof(double));
  stVKStiffnessMatrix->ComputeStiffnessMatrix(zero, K);
  free(zero);
  delete(stVKStiffnessMatrix);
}

LinearFEMForceModel::~LinearFEMForceModel()
{
  delete(K);
}

double LinearFEMForceModel::GetElasticEnergy(const double * u)
{
  return 0.5 * K->QuadraticForm(u);
}

void LinearFEMForceModel::GetInternalForce(const double * u, double * internalForces)
{
  K->MultiplyVector(u, internalForces);
}

void LinearFEMForceModel::GetTangentStiffnessMatrixTopology(SparseMatrix ** tangentStiffnessMatrix)
{
  *tangentStiffnessMatrix = new SparseMatrix(*K);
}

void LinearFEMForceModel::GetTangentStiffnessMatrix(const double * u, SparseMatrix * tangentStiffnessMatrix)
{
  *tangentStiffnessMatrix = *K;
} 

