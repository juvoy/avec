#include "injector.h"

cap::Injector::Injector(std::string processName, std::string dllPath)
{
    this->processName = processName;
    this->dllPath = dllPath;


}

BOOL cap::Injector::Inject()
{
    if (!this->GetProcessHandle(this->hProcess)) {
        Error::SetLastError(1);
        return false;
    }

    if (!this->AllocateMemory(this->pBaseAddress)) {
        Error::SetLastError(2);
        return false;
    }

    if (!this->CreateRemoteThread(this->hThreadHandle)) {
        Error::SetLastError(3);
        return false;
    }

    if (this->WaitForThreadExit()) {
        Error::SetLastError(4);
        return false;
    }

    DWORD hDll;
    if (!GetExitCodeThread(this->hThreadHandle, &hDll)) {
        Error::SetLastError(5);
        return false;
    }

    if (hDll == 0x00000000) {
        Error::SetLastError(6);
        return false;
    }

    this->Close();
    return true;
}

BOOL cap::Injector::GetProcessHandle(HANDLE& hProcess)  // https://stackoverflow.com/a/865201
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = manual::kernel32::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (manual::kernel32::Process32FirstA(snapshot, &entry) == TRUE)
    {
        while (manual::kernel32::Process32NextA(snapshot, &entry) == TRUE)
        {
            std::wstring cmp(this->processName.begin(), this->processName.end());
            if (!cmp.compare(entry.szExeFile))
            {
                hProcess = manual::kernel32::OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            }
        }
    }

    manual::kernel32::CloseHandle(snapshot);

    return hProcess == INVALID_HANDLE_VALUE ? FALSE : TRUE;
}

BOOL cap::Injector::AllocateMemory(LPVOID& baseAddress)
{
    baseAddress = manual::kernel32::VirtualAllocEx(this->hProcess, NULL, this->dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!baseAddress) {
        return FALSE;
    }

    BOOL ret = manual::kernel32::WriteProcessMemory(this->hProcess, baseAddress, this->dllPath.c_str(), this->dllPath.length() + 1, NULL);

    return ret;
}

BOOL cap::Injector::CreateRemoteThread(HANDLE& threadHandle)
{
    void* LoadLibraryA = (void*)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll").decrypt()), skCrypt("LoadLibraryA"));

    threadHandle = manual::kernel32::CreateRemoteThread(this->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, this->pBaseAddress, 0, NULL);

    return threadHandle == NULL ? FALSE : TRUE;
}

DWORD cap::Injector::WaitForThreadExit()
{
    return manual::kernel32::WaitForSingleObject(this->hThreadHandle, INFINITE);
}

VOID cap::Injector::Close()
{
    manual::kernel32::VirtualFreeEx(this->hProcess, this->pBaseAddress, sizeof(dllPath), MEM_RELEASE);
    manual::kernel32::CloseHandle(this->hThreadHandle);
    manual::kernel32::CloseHandle(this->hProcess);
}

void cap::Error::SetLastError(int c)
{
    code = c;
}

int cap::Error::GetLastError()
{
    return code;
}
