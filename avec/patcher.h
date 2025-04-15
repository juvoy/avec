#pragma once
#include <windows.h>
#include <string>
#include <iostream>

namespace resources {
	std::string ReadResource();
}

class Patcher {
public:
	Patcher(std::string path);

	bool Patch(std::string patch);
private:
	std::string path;
};