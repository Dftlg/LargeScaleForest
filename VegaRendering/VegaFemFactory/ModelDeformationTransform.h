#pragma once
#include <glm/glm.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "sceneObjectDeformable.h"
#include "ObjStruct.h"
#include "common.h"


//�����Ҳ�ܽ��ƵĿ���һ��model
class CModelDeformationTransform
{
public:
	CModelDeformationTransform() = default;
	~CModelDeformationTransform() = default;
	CModelDeformationTransform(const std::string& vFileName);
	void SaveDeformationVertexFromBaseModel(const double* u, int vDeformationSize, std::string vSaveFileName, int vtimeStepCounter);
	void ConvertVertex2mutileVerteices(Common::SFileData &vBaseFileDeformation);
	std::vector<BaseObjConstruct::SGroup> getGroups() { return m_Groups; }
	std::vector<std::vector<int>> getGroupsIndex(){ return m_GroupsIndex; }
	int getVerticesNumber() { return m_VerticesNumber; }
	std::vector<int> getVertexRepeat() { return m_VertexRepeatNumber; }

private:
	void __VertexFaceRelated();

	void __VertexRepeat();

	//objģ���е�mesh�ļ���
	ObjMesh *m_BaseFileMesh;
	//obj�ܶ�����
	int m_VerticesNumber;
	//һϵ�е�group�ļ���
	std::vector<BaseObjConstruct::SGroup> m_Groups;
	std::vector<std::vector<int>>m_GroupsIndex;
	std::vector<int> m_VertexRepeatNumber;
};	