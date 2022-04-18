#pragma once
/*
   Autor: Joel Chacón Castillo
   Date: 02/may/2017
   Metric obtained form the paper:
    "Using the Average Hausdorff Distance as a Performance Measure in Evolutionary Multiobjective Optimization."

    by Oliver Shutze, Xavier Esquivel et al.

*/
#ifndef Hausdorff_HPP
#define Hausdorff_HPP
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <chrono>

using namespace std;
class Average_Hausdorff
{
public:
    typedef vector<vector<double> > vvd;
    Average_Hausdorff();
    //      inline void setFile_ParetoFront(string File_ParetoFront){ this->File_ParetoFront = File_ParetoFront;}
    //      inline void setFile_ParetoFront(string File_Approximation){ this->File_Approximation = File_Approximation;}
    double do_Metric_Hausdorff(string File_ParetoFront, string File_Approximation, double p);
    double InvertedGenerationalDistance(vvd &ParetoFront, vvd &Approximation);
    double GenerationalDistance(vvd &ParetoFront, vvd &Approximation);
    void NormalizeDataSets(vvd &ParetoFront, vvd &Approximation, int M);
    vector<double> ComputeMinDistances(vvd &SetA, vvd &SetB);
    double EuclideanDistance(vector<double> &x, vector<double> &y);
    void ReadData(string FileName, vvd &Set, int &cols);
    inline void setNormalize(bool Normalize) { this->Normalize = Normalize; }

private:
    double p = 1;
    bool Normalize = false;

};
#endif

Average_Hausdorff::Average_Hausdorff()
{
}
void Average_Hausdorff::ReadData(string FileName, vvd &Set, int &cols)
{
    FILE *instream;
    int retval = 1;
    char newline[2];
    double number;
    int error = 0;

    if (FileName.empty()) {
        instream = stdin;
        FileName = "<stdin>"; /* used to diagnose errors.  */
    }
    else if (NULL == (instream = fopen(FileName.c_str(), "r"))) {
        //     errprintf ("%s: %s\n", FileName, strerror (errno));
        exit(EXIT_FAILURE);
    }

    do
    {
        cols = 0;
        vector<double> row;

        /* Beginning of a row */
        do
        {
            /* New column */
            cols++;

            if (fscanf(instream, "%lf", &number) != 1) {
                char buffer[64];
                fscanf(instream, "%60[^ \t\r\n]", buffer);
                //                    errprintf ("%s: line %d column %d: "
                //                               "could not convert string `%s' to double", 
                //                               filename, line, column, buffer);
                exit(EXIT_FAILURE);
            }
            row.push_back(number);

            fscanf(instream, "%*[ \t]");
            retval = fscanf(instream, "%1[\r\n]", newline);

        } while (!retval);


        Set.push_back(row);
        /* skip over successive empty lines */
        do {
            if (!fscanf(instream, "%1[#]%*[^\n\r]", newline))
                fscanf(instream, "%*[ \t]");
            retval = fscanf(instream, "%1[\r\n]", newline);
        } while (retval == 1);
    } while (retval != EOF);
}

double Average_Hausdorff::do_Metric_Hausdorff(string File_ParetoFront, string File_Approximation, double p)
{
    this->p = p;
    vvd ParetoFront, Approximation;
    int M;
    ReadData(File_ParetoFront, ParetoFront, M);
    ReadData(File_Approximation, Approximation, M);
    //The data-sets should be normalized, then the extremal points are obtained..
    if (this->Normalize)
        NormalizeDataSets(ParetoFront, Approximation, M);
    auto start = chrono::system_clock::now();
    double IGD = InvertedGenerationalDistance(ParetoFront, Approximation);
    double GD = GenerationalDistance(ParetoFront, Approximation);
    auto end = chrono::system_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end - start);
    //cout << time.count()<<"us "<<double(time.count())/1000<<"ms "<<double(time.count())/1000000<<"s" << endl;
    //cout << IGD << " - " << GD <<endl;
    return max(IGD, GD);
}

double Average_Hausdorff::InvertedGenerationalDistance(vvd &ParetoFront, vvd &Approximation)
{
    vector<double> MinDistances = ComputeMinDistances(ParetoFront, Approximation);
    int N = ParetoFront.size();
    double Sum = 0;
    for (int i = 0; i < N; i++)
        Sum += pow(MinDistances[i], this->p);
    return pow(Sum / N, 1.0 / this->p);
}

double Average_Hausdorff::GenerationalDistance(vvd &ParetoFront, vvd &Approximation)
{
    vector<double> MinDistances = ComputeMinDistances(Approximation, ParetoFront);
    double Sum = 0;
    int N = Approximation.size();
    for (int i = 0; i < N; i++)
        Sum += pow(MinDistances[i], this->p);
    return pow(Sum / N, 1.0 / this->p);
}

void Average_Hausdorff::NormalizeDataSets(vvd &ParetoFront, vvd &Approximation, int M)
{
    vector<double> Min(M, INFINITY), Max(M, -INFINITY);

    for (int i = 0; i < ParetoFront.size(); i++)
    {
        for (int j = 0; j < M; j++)
        {
            Min[j] = min(Min[j], ParetoFront[i][j]);
            Max[j] = max(Max[j], ParetoFront[i][j]);
        }
    }
    for (int i = 0; i < ParetoFront.size(); i++)
    {
        for (int j = 0; j < M; j++)
        {
            ParetoFront[i][j] = (ParetoFront[i][j] - Min[j]) / (Max[j] - Min[j]);
        }
    }
    for (int i = 0; i < Approximation.size(); i++)
    {
        for (int j = 0; j < M; j++)
        {
            Approximation[i][j] = (Approximation[i][j]) / (Max[j] - Min[j]);
        }
    }
}

vector<double> Average_Hausdorff::ComputeMinDistances(vvd &SetA, vvd &SetB)
{
    vector<double> MinDistances(SetA.size(), INFINITY);
    for (int i = 0; i < SetA.size(); i++)
    {
        for (int j = 0; j < SetB.size(); j++)
        {
            double distance = EuclideanDistance(SetA[i], SetB[j]);
            MinDistances[i] = min(distance, MinDistances[i]);
        }
    }
    return MinDistances;
}

double Average_Hausdorff::EuclideanDistance(vector<double> &x, vector<double> &y)
{
    double Sum = 0;
    for (int i = 0; i < x.size(); i++)
        Sum += (x[i] - y[i])*(x[i] - y[i]);
    return sqrt(Sum);
}
