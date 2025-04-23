/*
* CREDITS : https://github.com/KoljaGoldstein/cap/
*/

#pragma once
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <iostream>
#include <string>

#include "import.h"

namespace cap {

	class Injector {
	public:
		Injector(std::string processName, std::string dllPath);

		BOOL Inject();
	private:
		HANDLE hProcess;
		BOOL GetProcessHandle(HANDLE& hProcess);

		void* pBaseAddress;
		BOOL AllocateMemory(LPVOID& pBaseAddress);

		HANDLE hThreadHandle;
		BOOL CreateRemoteThread(HANDLE& hThreadHandle);

		DWORD WaitForThreadExit();

		VOID Close();
		std::string processName, dllPath;
	};

	class Error {
	public:
		static void SetLastError(int code);
		static int GetLastError();
	private:
		inline static int code;
	};
}