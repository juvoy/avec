#pragma once

#include <string>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>

#include <iostream>

#include "libraries/skcrypt/skcrypt.hpp"

namespace inet_import {
	typedef HINTERNET(WINAPI* CustomInternetOpenA)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
	typedef BOOL(WINAPI* CustomInternetCrackUrlA)(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags, LPURL_COMPONENTSA lpUrlComponents);
	typedef HINTERNET(WINAPI* CustomInternetConnectA)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort,	LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
	typedef HINTERNET(WINAPI* CustomHttpOpenRequestA)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
	typedef BOOL(WINAPI* CustomHttpSendRequestA)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength,LPVOID lpOptional, DWORD dwOptionalLength);

	inline CustomInternetCrackUrlA InternetCrackUrlA;
	inline CustomInternetOpenA InternetOpenA;
	inline CustomInternetConnectA InternetConnectA;
	inline CustomHttpOpenRequestA HttpOpenRequestA;
	inline CustomHttpSendRequestA HttpSendRequestA;

	void init();
}

class Request {
public:
	Request(std::string web_address, std::string headers);
	Request(std::string web_address, std::string headers, std::string body);

	Request(std::string web_address);
	Request(std::string web_address, std::map<std::string, std::string> headers, std::string body);
	Request(std::string web_address, std::map<std::string, std::string> headers);

	std::string Get();
	std::string Post();
private:
	std::string web_address, headers, body;

	HINTERNET hInternet = NULL, hConnect = NULL;
};