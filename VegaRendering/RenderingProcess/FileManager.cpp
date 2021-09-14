#include "FileManager.h"

bool FileManager::ReadFile(const std::string& filepath, unsigned char *& dataPtr, unsigned int& dataSize)
{
	//std::string finalPath = FileManager::GetFilePath(filepath);

#if PLATFORM_ANDROID

	AAsset* asset = AAssetManager_open(g_AndroidApp->activity->assetManager, finalPath.c_str(), AASSET_MODE_STREAMING);
	dataSize = AAsset_getLength(asset);
	dataPtr = new uint8[dataSize];
	AAsset_read(asset, dataPtr, dataSize);
	AAsset_close(asset);

#else

	FILE* file = fopen(filepath.c_str(), "rb");
	if (!file) {
		std::cout<<"File not found :"<<filepath.c_str();
		return false;
	}

	fseek(file, 0, SEEK_END);
	dataSize = (unsigned int)ftell(file);
	fseek(file, 0, SEEK_SET);

	if (dataSize <= 0) {
		fclose(file);
		std::cout << "File has no data"<<filepath.c_str();
		return false;
	}

	dataPtr = new unsigned char[dataSize];
	fread(dataPtr, 1, dataSize, file);
	fclose(file);

#endif

	return true;
}