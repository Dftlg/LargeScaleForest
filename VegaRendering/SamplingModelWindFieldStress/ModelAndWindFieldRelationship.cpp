#include "pch.h"
#include "ModelAndWindFieldRelationship.h"

#pragma optimize("", off)

CModelAndWindFieldRelationShip::CModelAndWindFieldRelationShip(const ObjMesh * objMesh_, const int resolution_[3], int depth_, Vec3d bmin_, Vec3d bmax_)
{
    m_ObjMesh = objMesh_;
    __init(resolution_, depth_, bmin_, bmax_);
}

CModelAndWindFieldRelationShip::CModelAndWindFieldRelationShip(const ObjMesh * objMesh_, const int resolution_[3], int depth_, double expansionFactor)
{
    m_ObjMesh = objMesh_;

    // build mesh bounding box
    Vec3d bmin_, bmax_;
    m_ObjMesh->getCubicBoundingBox(expansionFactor, &bmin_, &bmax_);

    __init(resolution_, depth_, bmin_, bmax_);
}

void CModelAndWindFieldRelationShip::__init(const int resolution_[3], int depth_, Vec3d bmin_, Vec3d bmax_)
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

void CModelAndWindFieldRelationShip::__searchNeighborVoxel(voxel & vcurrentVoxel, std::set<voxel> & vCheckedVoxels, std::vector<voxel>& vScheduledVoxels, int x, int y, int z)
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

void CModelAndWindFieldRelationShip::__calculateGroupVoxelsIndex()
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

void CModelAndWindFieldRelationShip::SaveCorrespondingVoxelOfTreeGroup(const std::string & vDirectionPath)
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
