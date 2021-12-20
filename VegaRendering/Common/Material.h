#pragma once
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "common.h"
//参考volumetricMesh.cpp

class CMaterial
{
public:
    Common::EMaterialType MaterialType;
    CMaterial()=default;
    virtual ~CMaterial() {};
    double Mass;
    double Nu;
};

class CENUMaterial:public CMaterial
{
public: 
    double E;
    CENUMaterial() = default;
    CENUMaterial(double vMass,double vE,double vNu)
    {
        MaterialType = Common::EMaterialType::ENU;
        Mass = vMass;
        E = vE;
        Nu = vNu;
    }
    ~CENUMaterial() = default;

};

class CORTHOTROPIC_N1Material :public CMaterial
{
public:
    CORTHOTROPIC_N1Material() = default;
    CORTHOTROPIC_N1Material(double vMass,double vE11,double vE22,double vE33,double vNu)
    {
        MaterialType = Common::EMaterialType::ORTHOTROPIC_N1;
        Mass = vMass;
        E11 = vE11;
        E22 = vE22;
        E33 = vE33;
        Nu = vNu;
    }
    double E11, E22, E33;
    ~CORTHOTROPIC_N1Material() = default;
};
