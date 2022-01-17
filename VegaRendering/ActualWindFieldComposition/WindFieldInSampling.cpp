#include "stdafx.h"
#include "WindFieldInSampling.h"


#pragma optimize("", off)

CWindFieldSampling::CWindFieldSampling(const ObjMesh * objMesh_, const int resolution_[3], int depth_, Vec3d bmin_, Vec3d bmax_)
{
    m_ObjMesh = objMesh_;
    __init(resolution_, depth_, bmin_, bmax_);
}

CWindFieldSampling::CWindFieldSampling(const ObjMesh * objMesh_, const int resolution_[3], int depth_, double expansionFactor, double vTimeStep)
{
    m_ObjMesh = objMesh_;

    // build mesh bounding box
    Vec3d bmin_, bmax_;
    m_ObjMesh->getCubicBoundingBox(expansionFactor, &bmin_, &bmax_);

    m_BoundBoxMin = bmin_;
    m_BoundBoxMax = bmax_;
    m_Side = m_BoundBoxMax - m_BoundBoxMin;
    m_inc[0] = m_Side[0] / m_Resolution[0];
    m_inc[1] = m_Side[1] / m_Resolution[1];
    m_inc[2] = m_Side[2] / m_Resolution[2];

    Vec3d tempSide = 2 * m_Side;

    m_Time = 0;
    m_TimeStep = vTimeStep;

    m_FarthestDistance = sqrt(pow(tempSide[0], 2) + pow(tempSide[1], 2));
}

//定向风是依靠一个原点以及一个方向构成，这样可以生成一条风向线，风的方向垂直与风向线，原点默认使用1.5倍模型最小点

void CWindFieldSampling::GenerateWindFieldMeshAndWindDirectionLine(const std::string & vFilePath, const std::string & vExternFileDirectory)
{
    m_FileWindConfig= GetForceConfigurate(vFilePath, vExternFileDirectory, m_Theta, m_Phi, m_LineConfig);

    m_WindDirectionLinePoint =glm::vec3(m_BoundBoxMin[0] * 1.5,m_BoundBoxMin[1]*1.5, m_BoundBoxMin[2] * 1.5);

    //m_Phi+90因为风向线与风向垂直

    m_WindDirectionLine = Common::SXYZLine(m_WindDirectionLinePoint, m_Phi + 90);

}

//已知一个风向线，以及风向线该线上每一个点是相同的sin叠加的波动函数，那么随着场景中任意一点计算与该风向线的距离，可以用距离计算将该sin函数的相位表示距离与sin函数的波动，产生一个波动的风场如
//i3d 2017 Responsive Real-Time  Grass Rendering for General 3D Scenes的定向风场

void CWindFieldSampling::GenerateWindFieldExternalForce(SceneObjectDeformable* & vtempSceneObjectDeformable, double * vof_ext)
{
    m_SceneObjectVoxel = vtempSceneObjectDeformable;

    for (auto i = 0; i < m_ObjMesh->getNumVertices(); i++)
    {    
        Vec3d tempVertice = m_SceneObjectVoxel->GetSingleVertexPositionFromBuffer(i);
        glm::vec3 tempGlmVertice = glm::vec3(tempVertice[0], tempVertice[1], tempVertice[2]);
        double tempDistance=Point2LineDistance(m_WindDirectionLine, tempGlmVertice);

        //这里假设从m_windDirectionLinePoint到2倍的m_BoundBox也就是2倍的m_Side的对角线距离会更改掉pi/2的相位//这里改为了2pi感觉会有更好的效果。

        //double vPhase = tempDistance / m_FarthestDistance * M_PI / 2;
        double vPhase = tempDistance / m_FarthestDistance * M_PI * 2;
        //vPhase = 0;

        double SumForce = 0;
        double externalForce[3] = {0,0,0};

        //vTime = 0;

        for (auto tempWindConfig : m_FileWindConfig)
        {
            SumForce+=GenerateSamlplingForceWithTime(m_Time, tempWindConfig[0],tempWindConfig[1], tempWindConfig[2]+vPhase, tempWindConfig[3]);

            //SumForce += GenerateSamlplingForceWithTime(0, 5, 1, 0, 0);
        }
        if(i==0)
        std::cout <<"Time:"<< m_Time <<"   "<< "EachPointForce:" <<SumForce << std::endl;

        //SumForce = 1;
        SetWorldConSyForce(SumForce, m_Theta, m_Phi, externalForce);

        /*vof_ext[3 * i + 0] = 25;
        vof_ext[3 * i + 1] = 25;
        vof_ext[3 * i + 2] = 25;*/
        vof_ext[3 * i + 0] += externalForce[0];
        vof_ext[3 * i + 1] += externalForce[1];
        vof_ext[3 * i + 2] += externalForce[2];
    }
    m_Time += m_TimeStep;
}

void CWindFieldSampling::__init(const int resolution_[3], int depth_, Vec3d bmin_, Vec3d bmax_)
{
    m_Resolution[0] = resolution_[0];
    m_Resolution[1] = resolution_[1];
    m_Resolution[2] = resolution_[2];
    m_Depth = depth_;
    m_BoundBoxMin = bmin_;
    m_BoundBoxMax = bmax_;

    //cout << "Entering obj mesh voxelization routine..." << endl;
    //cout << "Resolution is " << resolution[0] << " x " << resolution[1] << " x " << resolution[2] << " ..." << endl;

    //cout << "Checking if mesh is triangular... ";
    std::vector<Vec3i> triangles;
    m_ObjMesh->exportTriangles(triangles);

    //test
    //objMesh->exportTriangles(triangles,0);

    m_Side = m_BoundBoxMax - m_BoundBoxMin;
    m_inc[0] = m_Side[0] / m_Resolution[0];
    m_inc[1] = m_Side[1] / m_Resolution[1];
    m_inc[2] = m_Side[2] / m_Resolution[2];

    Vec3d tempSide = 2 * m_Side;

    m_FarthestDistance = sqrt(pow(tempSide[0], 2) + pow(tempSide[1], 2));
    //cout << "Scene bounding box is: " << bmin << " " << bmax << endl;
    //cout << "Computing voxels intersecting the model..." << endl;

    // iterate over all triangles
    // for every triangle, find the voxel containing its center of mass
    // then, grow the voxels until they don't intersect the triangle any more

    m_Voxels.clear(); // will contain voxels intersecting the triangles

    // local search helpers:
    std::set<voxel> checkedVoxels; // used to mark what voxels have already been visited
    std::vector<voxel> scheduledVoxels; // contains voxels still to be processed
    for (size_t i = 0; i < triangles.size(); i++)
    {
        Vec3i t = triangles[i];
        Vec3d p0 = m_ObjMesh->getPosition(t[0]);
        Vec3d p1 = m_ObjMesh->getPosition(t[1]);
        Vec3d p2 = m_ObjMesh->getPosition(t[2]);
        TriangleBasic triangle(p0, p1, p2);

        Vec3d center = 1.0 / 3 * (p0 + p1 + p2);
        Vec3d relCenter = center - m_BoundBoxMin;

        // find voxel containing center
        int vi, vj, vk;
        vi = (int)(relCenter[0] / m_inc[0]);
        vj = (int)(relCenter[1] / m_inc[1]);
        vk = (int)(relCenter[2] / m_inc[2]);

        checkedVoxels.clear();
        checkedVoxels.insert(voxel(vi, vj, vk));

        scheduledVoxels.clear();
        scheduledVoxels.push_back(voxel(vi, vj, vk));

        // while there are still some scheduled voxels:
        //   take one
        //   check if intersecting the triangle
        //   if yes
        //     add voxel to voxels
        //     queue all neighbors that haven't been visited yet

        while (!scheduledVoxels.empty())
        {
            voxel v = scheduledVoxels.back();
            scheduledVoxels.pop_back();

            // make bounding box for voxel
            Vec3d bbmin = m_BoundBoxMin + Vec3d(v.first * m_inc[0], v.second * m_inc[1], v.third * m_inc[2]);
            BoundingBox bbox(bbmin, bbmin + m_inc);

            if (triangle.doesIntersectBox(bbox)) // intersection test
            {
                // add the voxel to the final list of hits
                m_Voxels.insert(v);
                for (int iii = -1; iii <= 1; iii++)
                    for (int jjj = -1; jjj <= 1; jjj++)
                        for (int kkk = -1; kkk <= 1; kkk++)
                        {
                            if ((iii == 0) && (jjj == 0) && (kkk == 0))
                                continue;
                            __searchNeighborVoxel(v, checkedVoxels, scheduledVoxels, iii, jjj, kkk);
                        }
            }
        }

        // now, voxels contains all voxels that intersect the given triangle (plus everything from previous triangles)
    }

    //buildUniqueListOfFaces();
}

void CWindFieldSampling::__searchNeighborVoxel(voxel & vcurrentVoxel, std::set<voxel> & vCheckedVoxels, std::vector<voxel>& vScheduledVoxels, int x, int y, int z)
{
    voxel neighbor = voxel(vcurrentVoxel.first + (x), vcurrentVoxel.second + (y), vcurrentVoxel.third + (z));
    if ((neighbor.first >= 0) && (neighbor.first <= m_Resolution[0]) &&
        (neighbor.second >= 0) && (neighbor.second <= m_Resolution[1]) &&
        (neighbor.third >= 0) && (neighbor.third <= m_Resolution[2]))
    {
        if (vCheckedVoxels.find(neighbor) == vCheckedVoxels.end())
        {
            vCheckedVoxels.insert(neighbor);
            vScheduledVoxels.push_back(neighbor);
        }
    }
}

void CWindFieldSampling::__calculateGroupVoxelsIndex()
{
    //m_GroupVoxelsIndex.resize(m_GroupVoxels.size());
    for (int GroupIndex = 0; GroupIndex < m_GroupVoxels.size(); GroupIndex++)
    {
        std::vector<int> GroupVoxelArray;
        std::set<voxel>::iterator EachGroupVoxel;
        for (EachGroupVoxel = m_GroupVoxels[GroupIndex].begin(); EachGroupVoxel != m_GroupVoxels[GroupIndex].end(); EachGroupVoxel++)
        {
            std::set<voxel>::iterator aVoxel;
            int baseindex = 1;
            for (aVoxel = m_Voxels.begin(); aVoxel != m_Voxels.end(); aVoxel++)
            {
                if (*aVoxel == *EachGroupVoxel)
                {
                    GroupVoxelArray.push_back(baseindex);
                    break;
                }

                baseindex++;
            }
        }
        m_GroupVoxelsIndex.push_back(GroupVoxelArray);
    }

}

void CWindFieldSampling::SaveCorrespondingVoxelOfTreeGroup(const std::string & vDirectionPath)
{
    for (int RegionsNumber = 0; RegionsNumber < m_GroupVoxelsIndex.size(); RegionsNumber++)
    {
        std::string FilePath = vDirectionPath;
        FilePath += "/ObjMappingGroup";
        FilePath += std::to_string(RegionsNumber);
        FilePath += ".txt";
        std::ofstream fout(FilePath.c_str());
        for (int ChildRegionsIndex = 0; ChildRegionsIndex < m_GroupVoxelsIndex[RegionsNumber].size(); ChildRegionsIndex++)
        {
            fout << m_GroupVoxelsIndex[RegionsNumber][ChildRegionsIndex] << ",";
        }
        fout.close();
    }
}


#pragma optimize("", on)
