#include "import.h"


void manual::kernel32::init()
{
	HMODULE hKernel = GetModuleHandleA(skCrypt("Kernel32.dll"));

	CreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)GetProcAddress(hKernel, skCrypt("CreateToolhelp32Snapshot"));
	Process32FirstA = (PROCESS32FIRST)GetProcAddress(hKernel, skCrypt("Process32FirstW"));
	Process32NextA = (PROCESS32NEXT)GetProcAddress(hKernel, skCrypt("Process32NextW"));
	
	OpenProcess = (OPENPROCESS)GetProcAddress(hKernel, skCrypt("OpenProcess"));

	VirtualAllocEx = (VIRTUALALLOCEX)GetProcAddress(hKernel, skCrypt("VirtualAllocEx"));
	WriteProcessMemory = (WRITEPROCESSMEMORY)GetProcAddress(hKernel, skCrypt("WriteProcessMemory"));
	CloseHandle = (CLOSEHANDLE)GetProcAddress(hKernel, skCrypt("CloseHandle"));
	CreateRemoteThread = (CREATEREMOTETHREAD)GetProcAddress(hKernel, skCrypt("CreateRemoteThread"));
	WaitForSingleObject = (WAITFORSINGLEOBJECT)GetProcAddress(hKernel, skCrypt("WaitForSingleObject"));
	VirtualFreeEx = (VIRTUALFREEEX)GetProcAddress(hKernel, skCrypt("VirtualFreeEx"));

	CreateDirectoryW = (CREATEDIRECTORY)GetProcAddress(hKernel, skCrypt("CreateDirectoryW"));
	IsDebuggerPresent = (ISDEBUGGERPRESENT)GetProcAddress(hKernel, skCrypt("IsDebuggerPresent"));
}

void manual::shell32::init()
{
	HMODULE hShell32 = LoadLibraryA(skCrypt("Shell32.dll"));

	SHGetKnownFolderPath = (SHGETKNOWNFOLDERPATH)GetProcAddress(hShell32, skCrypt("SHGetKnownFolderPath"));
}
