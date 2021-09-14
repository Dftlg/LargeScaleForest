#include "ModelDeformationTransform.h"
#include <chrono>

CModelDeformationTransform::CModelDeformationTransform(const std::string & vFileName)
{
	//����һ���������model����model����ֱ�Ӷ�ȡobjģ��
	SceneObjectDeformable *m_BaseFileStruct = new SceneObjectDeformable(vFileName.c_str());
	//�ٽ���ȡ��mesh���ص���������model��mesh
	m_BaseFileMesh = m_BaseFileStruct->GetMesh();
	__VertexFaceRelated();
	//__VertexRepeat();
}

void CModelDeformationTransform::__VertexRepeat()
{
	m_VertexRepeatNumber=std::vector<int>(m_VerticesNumber);
	for (unsigned int i = 0; i < m_BaseFileMesh->getNumGroups(); i++)
	{
		for (unsigned int iFace = 0; iFace < m_BaseFileMesh->getGroup(i).getNumFaces(); iFace++)
		{
			ObjMesh::Face face = m_BaseFileMesh->getGroup(i).getFace(iFace);
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < m_VerticesNumber; k++)
				{
					if (k == face.getVertex(j).getPositionIndex())
					{
						m_VertexRepeatNumber[k]++;
						break;
					}
				}
			}
		}
	}
}

//��m_Groups�е����ݽ������
void CModelDeformationTransform::__VertexFaceRelated()
{
	m_VerticesNumber = m_BaseFileMesh->getNumVertices();
	//ѭ��objģ���е���group����
	for (unsigned int i = 0; i < m_BaseFileMesh->getNumGroups(); i++)
	{
		m_Groups.push_back(BaseObjConstruct::SGroup(m_BaseFileMesh->getGroup(i).getName(), m_BaseFileMesh->getGroup(i).getNumFaces()));
		std::vector<int> tempVertexIndex;
		std::vector<int> tempGroupIndex;
		//ѭ��ÿ��group��ÿ��face������
		for (unsigned int iFace = 0; iFace < m_BaseFileMesh->getGroup(i).getNumFaces(); iFace++)
		{
			ObjMesh::Face face = m_BaseFileMesh->getGroup(i).getFace(iFace);

			//ѭ��ÿ��face��ÿ������ĸ�����һ��face��������
			for (int j = 0; j < 3; j++)
			{
				tempVertexIndex.push_back(face.getVertex(j).getPositionIndex());
				tempGroupIndex.push_back(face.getVertex(j).getPositionIndex());
			}
			m_Groups[i].Faces.push_back(BaseObjConstruct::SFace(tempVertexIndex));
			tempVertexIndex.clear();
		}
		m_GroupsIndex.push_back(tempGroupIndex);
		tempGroupIndex.clear();
	}
	std::cout << "Finish Load Vega Vertex And Face Relationship" << std::endl;
}

//����ԭʼu����䰴���˳������Ķ���u
void CModelDeformationTransform::ConvertVertex2mutileVerteices(Common::SFileData &vBaseFileDeformation)
{
	if (m_VerticesNumber != vBaseFileDeformation.BaseFileDeformations.size()) std::cout << "the DataSet not equal to baseFile" << std::endl;

	//ѭ��group
	for (unsigned int i = 0; i < m_BaseFileMesh->getNumGroups(); i++)
	{
		Common::SFileDataGroup tempDataGroup(i);
		//ÿ��group�е�������
		for (unsigned int faceid = 0; faceid < m_Groups[i].FaceSize; faceid++)
		{
			//ÿ�����е����ж���
			for (unsigned int vertexindex = 0; vertexindex < 3; vertexindex++)
			{
				tempDataGroup.PositionsDeformation.push_back(vBaseFileDeformation.BaseFileDeformations[m_Groups[i].Faces[faceid].VertexIndex[vertexindex]]);
			}
		}
		//��һ֡������group��Ӧ�İ��������е�u���д洢
		vBaseFileDeformation.FileDeformation.push_back(tempDataGroup);
	}
}

void CModelDeformationTransform::SaveDeformationVertexFromBaseModel(const double* u, const int vDeformationSize, std::string vSaveFileName, int vtimeStepCounter)
{

#pragma region sava tree vertices deformations for a frame
	const size_t last_slash_idx = vSaveFileName.rfind('.txt');
	std::string FramesBlockFileName = vSaveFileName.substr(0, last_slash_idx - 3);
	FramesBlockFileName = FramesBlockFileName + "_" + std::to_string(vtimeStepCounter) + ".txt";
	std::ofstream connectionFile;
	connectionFile.open(FramesBlockFileName, std::ios::in | std::ios::app);
	for (unsigned int i = 0; i < vDeformationSize; i++)
	{
		connectionFile << u[3 * i + 0] << " " << u[3 * i + 1] << " " << u[3 * i + 2];
		connectionFile << "\n";
	}
#pragma endregion


#pragma region sava tree vertices deformations for all frames
	//if (!vSaveFileName.empty())
	//{
	//	char s[4096];
	//	FILE * file = fopen(vSaveFileName.c_str(), "ab");
	//	if (!file)
	//	{
	//		printf("Can't open output file: %s.\n", s);
	//	}
	//	else
	//	{
	//		float *deltaU = (float*)malloc(sizeof(float)*vDeformationSize * 3);
	//		for (unsigned int i = 0; i < vDeformationSize; i++)
	//		{
	//			deltaU[3 * i + 0] = float(u[3 * i + 0]);
	//			deltaU[3 * i + 1] = float(u[3 * i + 1]);
	//			deltaU[3 * i + 2] = float(u[3 * i + 2]);
	//		}
	//		
	//		fwrite(deltaU, sizeof(float)*vDeformationSize*3, 1, file);
	//		
	//		//sprintf(s, "Position%d", vtimeStepCounter);
	//		//fprintf(file, "%s \n", s);
	//		//sprintf(s, "%d", vDeformationSize);
	//		//fprintf(file, "%s \n", s);
	//		//for (unsigned int i = 0; i < vDeformationSize; i++)
	//		//{
	//		//	fprintf(file, "%.10lf %.10lf %.10lf ", u[3 * i + 0], u[3 * i + 1], u[3 * i + 2]);
	//		//	//fprintf(file, "\n");
	//		//}
	//		////fprintf(file, "%.10lf %.10lf %.10lf ", u[3 * 32464 + 0], u[3 * 32464 + 1], u[3 * 32464 + 2]);
	//		//fprintf(file, "\n");
	//	}
	//	fclose(file);
	//}
#pragma endregion
}