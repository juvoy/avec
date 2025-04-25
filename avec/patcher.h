#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>

namespace resources {
	std::string ReadResource();

	bool CopyResource(int, std::string, std::string);
}

class Patcher {
public:
	Patcher(std::string path);

	bool Patch(std::string patch);
private:
	std::string path;
};