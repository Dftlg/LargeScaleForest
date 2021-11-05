#pragma once
//#include <glm/vec3.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <io.h>
#include <assimp/mesh.h>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace Common
{
	enum EFileFramesType
	{
		OneRelatedFile,
		TwoRelatedFiles,
		FourRelatedFiles
	};
	constexpr size_t NumOfBoundingBoxVertices = 8;
	static inline glm::vec3 vec3_cast(const aiVector3D &vec3) { return glm::vec3(vec3.x, vec3.y, vec3.z); }
	static inline glm::vec2 vec2_cast(const aiVector3D &vec2) { return glm::vec2(vec2.x, vec2.y); } // it's aiVector3D because assimp's texture coordinates use that
	/*static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
	static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
	static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }*/
	//if this struct need not add normal or other info
	//Consider change this struct to glm::vec3
	static bool ModelHaveTangentAndBitangent = false;

	static bool InstanceTrees = true;

	static int ProductForceNumber = 5;

    /////each time change

    const int AllTreesNumber = 2;

    const int TreesTypeNumber = 1;

    const int TreesNumbers[TreesTypeNumber] = {2};
    //int SecondTypeTreesNumber = 5;

    //const int AllTreesNumber = 100;

    //const int TreesTypeNumber = 2;

    //const int TreesNumbers[TreesTypeNumber] = {60,40 };
    ////int SecondTypeTreesNumber = 5;
    /*const float ScaleTree[TreesTypeNumber] = { 0.7,1.5,1.0 };*/
   // const float ScaleTree[TreesTypeNumber] = { 0.3,2,1.0 };

	//const float ScaleTree[TreesTypeNumber] = { 0.35,0.6,0.5};
	const float ScaleTree[TreesTypeNumber] = { 0.35 };
	


	//const float ModelAxisYMove[TreesTypeNumber] = { 0.7,0.55,0.45 };
	//const float ModelAxisYMove[TreesTypeNumber] = { 0.7,0.45,0.45 };
	const float ModelAxisYMove[TreesTypeNumber] = { 0.5 };
	//const float ModelAxisYMove[TreesTypeNumber] = { 0.7,0.6,0.45 };

	const float ModelCutInTerrain[TreesTypeNumber] = { 9};
	//const float ModelCutInTerrain[TreesTypeNumber] = { 9.5,5,5 };

	static int MaxTimeStep = 60;

	static int readFrames = 60;

	static int DeformationObject=true;

	static int ForcesSampling = 5;

	static int uDeformationSampling = 10;

	static int CorrectuDeformationFrame = 20;

	//一个文件中存多少帧
	static int SamplingFrameNumber = 180;

	static int ProductFrameNumber = 1800000;

	//static int KErrorRange = 10;
	static int ExpandForceError = 1;
	static int KErrorRange = 20;
	static double VelocityErrorRange = 0.07;
	static double internalForceErrorRange = 50;

	static float Pi = 3.1415926;

	static bool GenerateRandomTreePosition=false;

	//use Geom Or Comp Shader calculate Normal
	static bool UseGeomOrCompCalculateNormal = false;

	static bool renderingGrass = false;
	static bool renderingLightSource = true;
    static bool renderingVegMesh = true;

    enum DrawType
    {
        TRIANGLES,
        LINES
    };

	struct SVertex
	{
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;

		SVertex() = default;
		SVertex(const const glm::vec3& vPosition, const glm::vec3& vNormal, const glm::vec2& vTexCoords, const glm::vec3& vTangent, const glm::vec3& vBitangent)
		{
			Position = vPosition;
			Normal = vNormal;
			TexCoords = vTexCoords;
			Tangent = vTangent;
			Bitangent = vBitangent;
		}

	};

	struct SAssociatedMassPoint
	{
		int AssociatedMassPointIndex;
		float RestLength;
		float SpringConstant;
		float DampingConstant;
	};

	struct SMassPoint
	{
		SMassPoint() = default;
		SMassPoint(const int& vMassIndex, const SVertex& vVertex, const float& vMassValue, const glm::vec3& vVelocity, const std::vector<SAssociatedMassPoint> vAssociatedMassPoint)
		{
			MassPointIndex = vMassIndex;
			Vertex = vVertex;
			MassValue = vMassValue;
			Velocity = vVelocity;
			AssociatedMassPoint = vAssociatedMassPoint;
		}
		int MassPointIndex;
		SVertex Vertex;
		float MassValue;
		glm::vec3 Velocity;
		//该定义替换位置
		//glm::vec3 Gravity = glm::vec3(0.0, 0.0, 0.0);
		std::vector<SAssociatedMassPoint> AssociatedMassPoint;
	};

	struct STexture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	struct SCameraInfo
	{
		glm::vec3 Eye;
		glm::vec3 LookAt;
		glm::vec3 Up;

		SCameraInfo() = default;
		SCameraInfo(const glm::vec3& vEye, const glm::vec3& vLookAt, const glm::vec3& vUp)
		{
			Eye = vEye;
			LookAt = vLookAt;
			Up = vUp;
		}

		bool operator ==(const SCameraInfo& vCameraInfo) const
		{
			return vCameraInfo.Eye == this->Eye && vCameraInfo.LookAt == this->LookAt && vCameraInfo.Up == this->Up;
		}
	};

	struct SWindow
	{
		float BottomLeftX = FLT_MAX;
		float BottomLeftY = FLT_MAX;
		float Width = FLT_MAX;
		float Height = FLT_MAX;

		SWindow() = default;
		SWindow(float vBottomLeftX, float vBottomLeftY, float vWidth, float vHeight)
		{
			BottomLeftX = vBottomLeftX;
			BottomLeftY = vBottomLeftY;
			Width = vWidth;
			Height = vHeight;
		}

		bool isNormalizedWindow() const
		{
			return BottomLeftX >= 0.0f && BottomLeftX <= 1.0f && Width > 0.0f && BottomLeftX + Width <= 1.0f
				&& BottomLeftY >= 0.0f && BottomLeftY <= 1.0f && Height > 0.0f && BottomLeftY + Height <= 1.0f;
		}
	};

	const size_t NumOfAxis = 3;

	//每一帧中的某一个group，由顶点集合组成
	struct SFileDataGroup
	{
		unsigned int GroupIndex;
		std::vector<glm::vec3> PositionsDeformation;
		SFileDataGroup(unsigned int vGroupIndex)
		{
			GroupIndex = vGroupIndex;
		}
		void clear() {
			PositionsDeformation.clear();
		}
	};

	struct vec
	{
		double x;
		double y;
		double z;
	};

	//一个文件中的每一帧
	struct SFileData
	{
		unsigned int FrameIndex;
		bool FileDeformationExist = false;
		//原始deltaU
		std::vector<glm::vec3> BaseFileDeformations;
		//按照面顶点顺序排列的u
		std::vector<SFileDataGroup> FileDeformation;
		double * singFrameDeformation;
		SFileData() = default;
		SFileData(unsigned int vFrameIndex)
		{
			FrameIndex = vFrameIndex;
		}
	};

	struct SForceDirection
	{
		int Theta;
		int Phi;
		SForceDirection() = default;
		SForceDirection(int vTheta, int vPhi)
		{
			Theta = vTheta;
			Phi = vPhi;
		}
	};

	//一个文件中帧的KVF数据，KVF数据既代表前面帧的结果，又可以用来判断后面帧的数据
	//树木朝向默认都为向右
	struct SpKVFData
	{
		int FrameIndex;

		//风的方向
		std::vector<SForceDirection> WindDirection;

		std::vector<int> Forces;
		std::vector<int> KLengths;
		std::vector<std::vector<double>> Kmatrix;

		std::vector<glm::vec3> InternalForces;
		std::vector<glm::vec3> Velocity;
	};

	struct SpDeformation
	{
		int FrameIndex;
		std::vector<glm::vec3> Deformation;
		SpDeformation(int vFrameIndex, std::vector<glm::vec3> & vDeformation)
		{
			FrameIndex = vFrameIndex;
			Deformation = vDeformation;
		}
	};

	//每一个是一个文件
	struct SFileFrames
	{
		//文件的名字
		std::string FileName;
		//文件的绝对路径
		std::string FilePath;
		bool isLoadDataSet = false;
		int Theta;
		int Phi;
        //力的sin函数部分
		std::vector<std::vector<double>> ForceFluctuationSequence;
        //力的基础部分
        std::vector<std::pair<int, int>> PolyLineForceSequence;
		//delta U数据
		std::vector<SFileData> Frames;
		std::vector<SpKVFData> KVFFrameDatas;
		//U 数据
		std::vector<SpDeformation> Deformations;
		SFileFrames() = default;
		SFileFrames(std::string vIndex, std::string vFilePath)
		{
			FileName = vIndex;
			FilePath = vFilePath;
		}
	};

	struct SMatchedDeformationFrames
	{
		std::string FileName;
		int FrameIndex;
		int CounterNumber;
		bool operator<(const SMatchedDeformationFrames &Deformation2) const
		{
			return CounterNumber > Deformation2.CounterNumber;
		}
	};

	

	//files gather
	struct SConnectedFemFiles
	{
		unsigned int ConnectedIndex;
		EFileFramesType Type;
		std::vector<SFileFrames*> FemDataset;
		SConnectedFemFiles() = default;
		SConnectedFemFiles(unsigned int vConnectedIndex)
		{
			ConnectedIndex = vConnectedIndex;
		}
	};

}
