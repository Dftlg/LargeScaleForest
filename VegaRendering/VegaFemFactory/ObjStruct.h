#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace BaseObjConstruct
{
	//标准的obj结构，face由一系列的顶点索引号组成
	struct SFace
	{
		std::vector<int> VertexIndex;
		SFace() = default;
		SFace(std::vector<int>& vVertexIndex)
		{
			VertexIndex = vVertexIndex;
		}
	};

	//group由一系列的面组成，group都有属于自己的面的个数以及名字
	struct SGroup
	{
		std::string FaceName;
		std::vector<SFace> Faces;
		unsigned int FaceSize;
		SGroup() = default;
		//use对应的名字
		SGroup(std::string vFaceName, unsigned int vFaceSize)
		{
			FaceName = vFaceName;
			FaceSize = vFaceSize;
		}
	};
}