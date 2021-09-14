/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "exactArithmetic" library , Copyright (C) 2018 USC                    *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code authors: Yijing Li, Jernej Barbic                                *
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

#include "planeER.h"

FastPlaneER::FastPlaneER(const Vec3ER & v0, const Vec3ER & v1, const Vec3ER & v2)
{
  Vec3ER e1 = v1 - v0, e2 = v2 - v0;
  dir = cross(e1, e2);
  d = dot(v0, dir);
}

int FastPlaneER::outside(const Vec3ER & v) const {
  auto s = ER_sign(dot(v,dir) - d);
  return s;
}

ER FastPlaneER::scaledDistance(const Vec3ER & v) const {
  return ER_abs(dot(v,dir) - d);
}
