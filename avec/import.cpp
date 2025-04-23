#include "import.h"


void manual::kernel32::init()
{
	HMODULE hKernel = GetModuleHandleA("Kernel32.dll");

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

	CreateDirectoryA = (CREATEDIRECTORYA)GetProcAddress(hKernel, skCrypt("CreateDirectoryA"));
	IsDebuggerPresent = (ISDEBUGGERPRESENT)GetProcAddress(hKernel, skCrypt("IsDebuggerPresent"));
}
