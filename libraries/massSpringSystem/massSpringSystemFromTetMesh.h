/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "massSpringSystem" library, Copyright (C) 2007 CMU, 2009 MIT,         *
 *                                           2018 USC                    *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Jernej Barbic, Daniel Schroeder                          *
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

#ifndef MASSSPRINGSYSTEMFROMTETMESH_H_
#define MASSSPRINGSYSTEMFROMTETMESH_H_

#include "massSpringSystem.h"

/*
  This class can generate a mass-spring system from the given tet mesh: 
  each tet mesh vertex becomes a mass point, and each edge becomes a spring.
  See also massSpringSystem.h
*/

class TetMesh;

class MassSpringSystemFromTetMesh
{
public:

  static int GenerateMassSpringSystem(TetMesh * tetMesh, MassSpringSystem ** massSpringSystem, double density, double tensileStiffness, double damping, int addGravity=0);
};

#endif

