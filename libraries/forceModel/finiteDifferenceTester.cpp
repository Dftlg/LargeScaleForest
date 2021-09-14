/*************************************************************************
 *                                                                       *
 * Vega FEM Simulation Library Version 4.0                               *
 *                                                                       *
 * "forceModel" library , Copyright (C) 2007 CMU, 2009 MIT, 2018 USC     *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Yijing Li                                                *
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

#include "finiteDifferenceTester.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <numeric>
#ifdef USE_TBB
  #include <tbb/tbb.h>
#endif
#include "range.h"
using namespace std;

static double squaredEuclideanNorm(const std::vector<double> & v)
{
  double sum = 0.0;
  for(size_t i = 0; i < v.size(); i++)
     sum += v[i] * v[i];
  return sum;
}

static double EuclideanNorm(const std::vector<double> & v)
{
  return sqrt(squaredEuclideanNorm(v));
}

// the Euclidean distance between the two vector of the same size
static double squaredEuclideanDistance(const std::vector<double> & v1, const std::vector<double> & v2)
{
  double sum = 0.0;
  assert(v1.size() == v2.size());
  for(size_t i = 0; i < v1.size(); i++)
  {
    double value = (v1[i] - v2[i]);
    sum += value * value;
  }
  return sum;
}

static double EuclideanDistance(const std::vector<double> & v1, const std::vector<double> & v2)
{
  return sqrt(squaredEuclideanDistance(v1,v2));
}


FiniteDifferenceTester::FiniteDifferenceTester(ForceModel * fm, double timestep, Mode m, int nt)
 : forceModel(fm), h(timestep), mode(m), numThreads(nt)
{
  r = forceModel->Getr();
  analyticForce.resize(r);
  finiteForce.resize(r);

  forceModel->GetTangentStiffnessMatrixTopology(&stiffnessMatrix0);
  transposedStiffness0 = stiffnessMatrix0->Transpose(r);
  unsymmetricStiffness = new SparseMatrix(*stiffnessMatrix0);
}

FiniteDifferenceTester::~FiniteDifferenceTester()
{
}

double FiniteDifferenceTester::testInternalForce(const double * u)
{
  double e0 = forceModel->GetElasticEnergy((double*)u);
  analyticForce.assign(r, 0.);
  forceModel->GetInternalForce((double*)u, analyticForce.data());
  // Warning: multi-threading works only when forceModel->GetElasticEnergy is parallel-safe
  #ifdef USE_TBB
  tbb::parallel_for(tbb::blocked_range<int>(0, r), [&](const tbb::blocked_range<int> & rng)
  {
  #else
  Range<int> rng(0, r);
  #endif
    vector<double> localDispBuffer(r);
    memcpy(localDispBuffer.data(), u, sizeof(double) * r);
    for (int j = rng.begin(); j != rng.end(); ++j) // j: [0, r)
    {
      double oldvalue = localDispBuffer[j];
      if (mode == FIVE_POINT)
      {
        localDispBuffer[j] = oldvalue - 2*h;
        double eb2h = forceModel->GetElasticEnergy(localDispBuffer.data());
        localDispBuffer[j] = oldvalue - h;
        double ebh = forceModel->GetElasticEnergy(localDispBuffer.data());
        localDispBuffer[j] = oldvalue + h;
        double efh = forceModel->GetElasticEnergy(localDispBuffer.data());
        localDispBuffer[j] = oldvalue + 2*h;
        double ef2h = forceModel->GetElasticEnergy(localDispBuffer.data());
        finiteForce[j] = (-ef2h + 8 * efh - 8 * ebh + eb2h) / (12 * h);
      }
      else
      {
        localDispBuffer[j] = oldvalue + h;
        double e = forceModel->GetElasticEnergy(localDispBuffer.data());
        finiteForce[j] = (e - e0) / h;
      }
      localDispBuffer[j] = oldvalue;
    }
    #ifdef USE_TBB
  }, tbb::static_partitioner());
  #endif

  double alen = EuclideanNorm(analyticForce);
  double dist = EuclideanDistance(analyticForce, finiteForce);
  double relErr = (alen == 0 ? dist : dist / alen);

  return relErr;
}

double FiniteDifferenceTester::testStiffnessMatrix(const double * u, double * relativeErrorOnUnsymmetry)
{

  vector<double> & force0 = analyticForce;
  force0.assign(r, 0.);
  stiffnessMatrix0->ResetToZero();
  forceModel->GetForceAndMatrix((double*)u, force0.data(), stiffnessMatrix0);
  double frobNorm = stiffnessMatrix0->GetFrobeniusNorm();
  transposedStiffness0->AssignTransposedMatrix(*stiffnessMatrix0);
  if (relativeErrorOnUnsymmetry)
    *unsymmetricStiffness = *stiffnessMatrix0 - *transposedStiffness0;

//  double maxAbsEntry = stiffnessMatrix0->GetMaxAbsEntry();

  vector<double> maxRelDiffInEachColumn(r, 0.), sumErrorEachColumn(r, 0.);

  // Warning: multi-threading works only when forceModel->GetInternalForce is parallel-safe
  // Warning: multi-threading works only when forceModel->GetElasticEnergy is parallel-safe
  #ifdef USE_TBB
  tbb::parallel_for(tbb::blocked_range<int>(0, r), [&](const tbb::blocked_range<int> & rng)
  {
  #else
  Range<int> rng(0, r);
  #endif
    vector<double> localDispBuffer(3*r); // this buffer stores three vector of size r: disp, force and force2
    memcpy(localDispBuffer.data(), u, sizeof(double) * r);
    for (int j = rng.begin(); j != rng.end(); ++j) // j: [0, r)
    {
      double oldvalue = localDispBuffer[j];
      double * dispBuffer = localDispBuffer.data();
      double * forceBuffer = localDispBuffer.data()+r;
      double * forceBuffer2 = localDispBuffer.data()+2*r;
      if (mode == FIVE_POINT)
      {
        dispBuffer[j] = oldvalue - 2*h;
        forceModel->GetInternalForce(dispBuffer, forceBuffer); // forceBuffer stores b2h

        dispBuffer[j] = oldvalue - h;
        forceModel->GetInternalForce(dispBuffer, forceBuffer2); // forceBuffer2 stores bh
        for(int k = 0; k < r; k++)
          forceBuffer[k] = forceBuffer[k] - 8 * forceBuffer2[k]; // forceBuffer stores - 8 * bh + b2h

        dispBuffer[j] = oldvalue + h;
        forceModel->GetInternalForce(dispBuffer, forceBuffer2); // forceBuffer2 stores fh
        for(int k = 0; k < r; k++)
          forceBuffer[k] += 8 * forceBuffer2[k]; // forceBuffer stores 8 * fh - 8 * bh + b2h

        dispBuffer[j] = oldvalue + 2*h;
        forceModel->GetInternalForce(dispBuffer, forceBuffer2); // forceBuffer2 stores f2h
        for(int k = 0; k < r; k++)
          forceBuffer[k] = (forceBuffer[k] - forceBuffer2[k]) / (12 * h); // ( -ef2h + 8 * fh - 8 * bh + b2h ) / (12 h)
      }
      else
      {
        localDispBuffer[j] = oldvalue + h;
        forceModel->GetInternalForce(dispBuffer, forceBuffer);
        for(int k = 0; k < r; k++)
        {
          forceBuffer[k] = (forceBuffer[k] - force0[k]) / h; // forceBuffer now contains jth column of the stiffness matrix
        }
      }

      // compare with jth row of transposedStiffness0
      int len = transposedStiffness0->GetRowLength(j);
      for(int k = 0; k < len; k++)
      {
        int index = transposedStiffness0->GetColumnIndex(j, k);
        double value = transposedStiffness0->GetEntry(j, k);
        double dif = value - forceBuffer[index];
        sumErrorEachColumn[j] += dif * dif;
  //      double absval = fabs(value);
  //      double reldif = (absval == 0 ? dif : dif / absval);
  //      if (dif > maxRelDiffInEachColumn[j]) maxRelDiffInEachColumn[j] = reldif;
      }

      localDispBuffer[j] = oldvalue;
    }
  #ifdef USE_TBB
  }, tbb::static_partitioner());
  #endif

//  double maxRelDif = *max_element(maxRelDiffInEachColumn.begin(), maxRelDiffInEachColumn.end());
  double sumErr = accumulate(sumErrorEachColumn.begin(), sumErrorEachColumn.end(), 0.);
  sumErr = sqrt(sumErr);
  double relErr = (frobNorm == 0 ? sumErr : sumErr / frobNorm);

  if (relativeErrorOnUnsymmetry)
  {
    double unsymErr =  unsymmetricStiffness->GetFrobeniusNorm();
    *relativeErrorOnUnsymmetry = (frobNorm == 0 ? unsymErr : unsymErr / frobNorm);
  }
  return relErr;
}

