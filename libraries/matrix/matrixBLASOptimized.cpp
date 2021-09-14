/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "matrix" library , Copyright (C) 2007 CMU, 2009 MIT, 2018 USC         *
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

#include "matrixMacros.h"
#include "matrixBLAS.h"

//void cblas_scopy(const int N, const float *X, const int incX, float *Y, const int incY); 


template<class real>
real * SumMatrices(int m, int n, const real * mtx1, const real * mtx2, real * output)
{
  real * target = output;
  if (target == NULL)
    target = (real*) malloc (sizeof(real) * m * n);

  _xcopy<sizeof(real)==sizeof(float)>::f(m*n, mtx1, 1, target, 1);   
  _xaxpy<sizeof(real)==sizeof(float)>::f(m*n, 1.0, mtx2, 1, target, 1);   

/*
  for(int i=0; i<m*n; i++)
    target[i] = mtx1[i] + mtx2[i];
*/
  return target;
}

template<class real>
real * SubtractMatrices(int m, int n, const real * mtx1, const real * mtx2, real * output)
{
  real * target = output;
  if (target == NULL)
    target = (real*) malloc (sizeof(real) * m * n);

  _xcopy<sizeof(real)==sizeof(float)>::f(m*n, mtx1, 1, target, 1);   
  _xaxpy<sizeof(real)==sizeof(float)>::f(m*n, -1.0, mtx2, 1, target, 1);   

/*
  for(int i=0; i<m*n; i++)
    target[i] = mtx1[i] - mtx2[i];
*/
  return target;
}

template<class real>
real * MultiplyMatrices(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  real * target = output;
  if (target == NULL)
    target = (real*) malloc (sizeof(real) * m * n);

  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, p, 1.0, mtx1, m, mtx2, p, 0.0, target, m);

/*
  for(int i=0; i<m; i++)
    for(int j=0; j<n; j++)
    {
      real entry = 0;
      for(int k=0; k<p; k++)
        entry += mtx1[ELT(m,i,k)] * mtx2[ELT(p, k, j)];
      target[ELT(m, i, j)] = entry;
    }
*/
  return target;
}

template<class real>
void MultiplyMatricesAdd(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, p, 1.0, mtx1, m, mtx2, p, 1.0, output, m);
}

template<class real>
void MultiplyMatricesSub(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, p, -1.0, mtx1, m, mtx2, p, 1.0, output, m);
}

template<class real>
real * MultiplyMatricesT(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  real * target = output;
  if (target == NULL)
    target = (real*) malloc (sizeof(real) * n * m);

  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasTrans, CblasNoTrans, m, n, p, 1.0, mtx1, p, mtx2, p, 0.0, target, m);

/*
  for(int i=0; i<m; i++)
    for(int j=0; j<n; j++)
    {
      real entry = 0;
      for(int k=0; k<p; k++)
        entry += mtx1[ELT(p, k, i)] * mtx2[ELT(p, k, j)];
      target[ELT(m, i, j)] = entry;
    }
*/
  return target;
}

template<class real>
void MultiplyMatricesTAdd(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasTrans, CblasNoTrans, m, n, p, 1.0, mtx1, p, mtx2, p, 1.0, output, m);
}

template<class real>
void MultiplyMatricesTSub(int m, int p, int n, const real * mtx1, const real * mtx2, real * output)
{
  _xgemm<sizeof(real)==sizeof(float)>::f(CblasColMajor, CblasTrans, CblasNoTrans, m, n, p, -1.0, mtx1, p, mtx2, p, 1.0, output, m);
}

template<class real>
real * ScalarMultiplyMatrix(int m, int n, real alpha, const real * mtx, real * output)
{
  real * target = output;
  if (target == NULL)
    target = (real*) malloc (sizeof(real) * m * n);

  _xcopy<sizeof(real)==sizeof(float)>::f(m*n, mtx, 1, target, 1);   
  _xscal<sizeof(real)==sizeof(float)>::f(m*n, alpha, target, 1);
  
/*
  for(int i=0; i<m*n; i++)
    target[i] = alpha * mtx[i];
*/

  return target;
}

// computes Euclidean norm of a vector
template <class real>
real VectorNorm(int m, const real * vec)
{
  return _xnrm2<sizeof(real)==sizeof(float)>::f(m, vec, 1);
}

