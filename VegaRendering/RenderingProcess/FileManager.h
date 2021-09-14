#pragma once

#include <string>
#include <iostream>

class FileManager
{
public:
	static bool ReadFile(const std::string& filepath, unsigned char *& dataPtr, unsigned int& dataSize);

	static std::string GetFilePath(const std::string& filepath);
};