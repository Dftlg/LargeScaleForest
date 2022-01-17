#pragma once    
#include<iostream>
#include <fstream>
#include<sstream>
#include<ctime>
#include<random>
#include<math.h>
#include <Windows.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include<sstream>
#include<algorithm>
#include "common.h"
#include <glm/glm.hpp>

#define M_PI 3.141592653589793238462643

int MaxElement(std::vector<int> &vDataSet);

int MinElement(std::vector<int> &vDataSet);

bool isDataExist(int number, std::vector<int> &vDataSet);

int RandomGenerate();

std::vector<int> GenerateSamplingForce(int vSize, int vAmplitude, float vFrequency, double vPhase, int vYpluse, int wavelength = 2);

std::vector<std::vector<double>> GetForceConfigurate(const std::string & vFilePath, const std::string &vExternFile, int &vTheta, int &vPhi, std::vector<std::pair<int, int>>& voLineForceConfig);

std::vector<int> GenerateLineForce(int vFrameNumber, std::vector<std::pair<int, int>>& vForceConfig);

std::vector<std::pair<int, int>> sGetLineForceConfigurate(const std::string &vExternFile);

std::vector<int> LineSamplineForce(std::pair<int, int>& vfirstPoint, std::pair<int, int>& vSecondPoint);

void GetStemAndLeafStreePointsConfigurate(const std::string& vFilePath, int& vStemPullvertexNums, std::pair<std::vector<int>, std::vector<float>>&vStreePointAndScale, std::vector<std::pair<int, int>>& vForceDeviationDirection, std::vector<float>&vFrequency, std::vector<float>&vPhase);

std::vector<std::pair<double, double>> RandomTreePositionGenerate(int vTreeNumber);

std::vector<std::pair<double, double>> StableTreePositionGenerate(int vTreesNumber);

void TransformCartesianCorrdinate2SphericalCorrdinate(glm::vec3 &vStartPoint, glm::vec3 &vEndPoint, double &voRadius, int& voTheta, int& voPhi);

glm::vec3 GenerateRamdomScale();

double OneNumberRangeError(float vNumber, int vControlFloatPosition=0, int vRange=1);

double Point2LineDistance(Common::SXYZLine& vLine, glm::vec3 &Point);

double GenerateSamlplingForceWithTime(double time, int vAmplitude, float vFrequency, double vPhase, int vYpluse);

void SetWorldConSyForce(float vforceDegree, double vforceDirectionTheta, double vforceDirectionPhi, double voforce[3]);
