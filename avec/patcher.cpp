#include "patcher.h"

Patcher::Patcher(std::string path)
{
	this->path = path;
}

bool Patcher::Patch(std::string patch)
{
	HANDLE hRes = BeginUpdateResourceA(this->path.c_str(), FALSE);
	if (!hRes) {
		return false;
	}

	BOOL result = UpdateResourceA(hRes, "text", MAKEINTRESOURCEA(101), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPVOID)patch.c_str(), (DWORD)patch.length());

	if (!result) {
		EndUpdateResource(hRes, TRUE);
		return false;
	}

	if (!EndUpdateResourceA(hRes, FALSE)) {
		return false;
	}

	return true;
}


std::string resources::ReadResource()
{
	HMODULE hModule = GetModuleHandleA(NULL);
	HRSRC hRes = FindResourceA(hModule, MAKEINTRESOURCEA(101), "text");
	if (!hRes) {
		std::cout << GetLastError() << std::endl;
		return "N/A";
	}

	HGLOBAL hData = LoadResource(hModule, hRes);
	if (!hData) {
		std::cout << GetLastError() << std::endl;
		return "N/A";
	}


	LPVOID pData = LockResource(hData);
	DWORD size = SizeofResource(hModule, hRes);
	std::string str(static_cast<const char*>(pData), size);
	
	return str;
}

bool resources::CopyResource(int resource, std::string resourceType, std::string path)
{
	HRSRC hRes = FindResourceA(NULL, MAKEINTRESOURCEA(resource), resourceType.c_str());
	if (!hRes) {
		std::cout << "FindResourceA failed: " << GetLastError() << std::endl;
		return 1;
	}

	HGLOBAL hData = LoadResource(NULL, hRes);
	DWORD size = SizeofResource(NULL, hRes);
	void* pResData = LockResource(hData);

	std::ofstream outFile(path, std::ios::binary);
	outFile.write(static_cast<const char*>(pResData), size);
	outFile.close();
	return false;
}
