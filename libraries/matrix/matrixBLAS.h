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

/*
  Wrappers to matrix BLAS routines.
  See also matrix.h.
*/

#ifndef _MATRIX_BLAS_H_
#define _MATRIX_BLAS_H_

#include <stdlib.h>
#include "lapack-headers.h"

template<bool C>
class _xcopy {};

template<>
class _xcopy<true>
{
public:
	inline static void f(const int N, const float * X, const int incX, float * Y, const int incY)
	{
		cblas_scopy(N, X, incX, Y, incY);
	}
};

template<>
class _xcopy<false>
{
public:
	inline static void f(const int N, const double * X, const int incX, double * Y, const int incY)
	{
		cblas_dcopy(N, X, incX, Y, incY);
	}
};

//void cblas_saxpy(const int N, const float alpha, const float *X, const int incX, float *Y, const int incY);
template<bool C>
class _xaxpy {};

template<>
class _xaxpy<true>
{
public:
	inline static void f(const int N, float alpha, const float * X, const int incX, float * Y, const int incY)
	{
		cblas_saxpy(N, alpha, X, incX, Y, incY);
	}
};

template<>
class _xaxpy<false>
{
public:
	inline static void f(const int N, double alpha, const double * X, const int incX, double * Y, const int incY)
	{
		cblas_daxpy(N, alpha, X, incX, Y, incY);
	}
};

//void cblas_sscal(const int N, const float alpha, float *X, const int incX); 
template<bool C>
class _xscal {};

template<>
class _xscal<true>
{
public:
	inline static void f(const int N, const float alpha, float * X, const int incX)
	{
		cblas_sscal(N, alpha, X, incX);
	}
};

template<>
class _xscal<false>
{
public:
	inline static void f(const int N, const double alpha, double * X, const int incX)
	{
		cblas_dscal(N, alpha, X, incX);
	}
};

//void cblas_snrm2(const int N, float * X, const int incX); 
template<bool C>
class _xnrm2 {};

template<>
class _xnrm2<true>
{
public:
	inline static float f(const int N, const float * X, const int incX)
	{
		return cblas_snrm2(N, X, incX);
	}
};

template<>
class _xnrm2<false>
{
public:
	inline static double f(const int N, const double * X, const int incX)
	{
		return cblas_dnrm2(N, X, incX);
	}
};

//void cblas_sgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const float alpha, const float *A, const int lda, const float *B, const int ldb, const float beta, float *C, const int ldc);
template<bool C>
class _xgemm {};

template<>
class _xgemm<true>
{
public:
	inline static void f(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const float alpha, const float *A, const int lda, const float *B, const int ldb, const float beta, float *C, const int ldc)
	{
		cblas_sgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
	}
};

template<>
class _xgemm<false>
{
public:
	inline static void f(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const double alpha, const double *A, const int lda, const double *B, const int ldb, const double beta, double *C, const int ldc)
	{
		cblas_dgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
	}
};

template<class real>
real * SumMatrices(int m, int n, const real * mtx1, const real * mtx2, real * output = NULL);

template<class real>
real * SubtractMatrices(int m, int n, const real * mtx1, const real * mtx2, real * output = NULL);

// output = mtx1 * mtx2
// mtx1 is m x p
// mtx2 is p x n
template<class real>
real * MultiplyMatrices(int m, int p, int n, const real * mtx1, const real * mtx2, real * output = NULL);
// output += mtx1 * mtx2
template<class real>
void MultiplyMatricesAdd(int m, int p, int n, const real * mtx1, const real * mtx2, real * output);
// output -= mtx1 * mtx2
template<class real>
void MultiplyMatricesSub(int m, int p, int n, const real * mtx1, const real * mtx2, real * output);

// output = trans(mtx1) * mtx2
// trans(mtx1) is m x p
// mtx2 is p x n
template<class real>
real * MultiplyMatricesT(int m, int p, int n, const real * mtx1, const real * mtx2, real * output = NULL);
// output += trans(mtx1) * mtx2
template<class real>
void MultiplyMatricesTAdd(int m, int p, int n, const real * mtx1, const real * mtx2, real * output);
// output -= trans(mtx1) * mtx2
template<class real>
void MultiplyMatricesTSub(int m, int p, int n, const real * mtx1, const real * mtx2, real * output);

template<class real>
real * ScalarMultiplyMatrix(int m, int n, real alpha, const real * mtx, real * output = NULL);

// transposes the matrix (without making a separate copy)
template <class real>
void InPlaceTransposeMatrix(int m, int n, real * mtx);

// computes Euclidean norm of a vector
template <class real>
real VectorNorm(int m, const real * vec);

#endif

