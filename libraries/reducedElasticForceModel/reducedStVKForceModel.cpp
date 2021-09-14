/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "elasticForceModel" library , Copyright (C) 2007 CMU, 2009 MIT,       *
 *                                                       2018 USC        *
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

#include "reducedStVKForceModel.h"

ReducedStVKForceModel::ReducedStVKForceModel(StVKReducedInternalForces * stVKReducedInternalForces_, StVKReducedStiffnessMatrix * stVKStiffnessMatrix_): stVKReducedInternalForces(stVKReducedInternalForces_), stVKStiffnessMatrix(stVKStiffnessMatrix_), useScale(0), scale(1.0), own_stVKStiffnessMatrix(false)
{
  r = stVKReducedInternalForces->Getr();
  if (stVKStiffnessMatrix == NULL)
  {
    own_stVKStiffnessMatrix = true;
    stVKStiffnessMatrix = new StVKReducedStiffnessMatrix(stVKReducedInternalForces);
  }
}

ReducedStVKForceModel::~ReducedStVKForceModel()
{
  if (own_stVKStiffnessMatrix)
    delete(stVKStiffnessMatrix);
}

void ReducedStVKForceModel::GetInternalForce(double * q, double * internalForces)
{
  stVKReducedInternalForces->Evaluate(q, internalForces);
  if (useScale)
  {
    for(int i=0; i<r; i++)
      internalForces[i] *= scale;
  }
}

void ReducedStVKForceModel::GetTangentStiffnessMatrix(double * q, double * tangentStiffnessMatrix)
{
  stVKStiffnessMatrix->Evaluate(q, tangentStiffnessMatrix);
  if (useScale)
  {
    int r2 = r * r;
    for(int i=0; i<r2; i++)
      tangentStiffnessMatrix[i] *= scale;
  }
}

