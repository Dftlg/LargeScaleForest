#pragma once
#include <glm/glm.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "sceneObjectDeformable.h"
#include "ObjStruct.h"
#include "common.h"


//这个类也能近似的看成一个model
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

	//obj模型中的mesh的集合
	ObjMesh *m_BaseFileMesh;
	//obj总顶点数
	int m_VerticesNumber;
	//一系列的group的集合
	std::vector<BaseObjConstruct::SGroup> m_Groups;
	std::vector<std::vector<int>>m_GroupsIndex;
	std::vector<int> m_VertexRepeatNumber;
};	