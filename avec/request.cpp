#include "request.h"

Request::Request(std::string web_address) : Request(web_address, "")
{
}


Request::Request(std::string web_address, std::map<std::string, std::string> headers)
{
    std::string header = "";
    for (auto const& [key, val] : headers)
    {
        header += key;
        header += ": ";
        header += val;
        header += "\r\n";
    }

    this->headers = header;
    this->web_address = web_address;
}

Request::Request(std::string web_address, std::map<std::string, std::string> headers, std::string body) : Request(web_address, headers)
{
    this->body = body;
}


Request::Request(std::string web, std::string h) : Request(web, h, "")
{

}

Request::Request(std::string web_address, std::string headers, std::string body)
{
    this->web_address = web_address;
    this->headers = headers;
    this->body = body;
}

std::string Request::Get()
{
    std::string returnBody = "";

    CHAR buffer[4096];
    DWORD bytesRead;
    hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        return "";
    }

    std::wstring web_address(this->web_address.begin(), this->web_address.end());
    std::wstring header(this->headers.begin(), this->headers.end());
    hConnect = InternetOpenUrlW(hInternet, web_address.c_str(), header.c_str(), 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0); // DONT CHANGE TO ASCII IT DOESNT WORK WITH ASCII
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }


    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        returnBody += buffer;
    }

    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return returnBody;
}

std::string Request::Post()
{
    hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
        return "";
    }

    // Convert URL to wide string
    std::wstring web_address(this->web_address.begin(), this->web_address.end());

    // Open HTTP session
    URL_COMPONENTSW urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(URL_COMPONENTSW);

    wchar_t hostName[256] = { 0 };
    wchar_t urlPath[1024] = { 0 };

    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName) / sizeof(wchar_t);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(wchar_t);


    if (!InternetCrackUrlW(web_address.c_str(), 0, 0, &urlComp)) {
        std::cerr << "InternetCrackUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    HINTERNET hSession = InternetConnectW(hInternet, urlComp.lpszHostName, urlComp.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

    if (!hSession) {
        std::cerr << "InternetConnect failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    LPCWSTR acceptTypes[] = { L"*/*", NULL };
    HINTERNET hRequest = HttpOpenRequestW(hSession, L"POST", urlComp.lpszUrlPath, NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        std::cerr << "HttpOpenRequest failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hSession);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::wstring headers(this->headers.begin(), this->headers.end());
    LPCSTR postData = this->body.c_str();

    if (!HttpSendRequestW(hRequest, headers.c_str(), (DWORD)headers.length(), (LPVOID)postData, static_cast<DWORD>(this->body.size()))) {
        std::cerr << "HttpSendRequest failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hSession);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string ret = "";

    CHAR buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        ret += buffer;
    }

    if (GetLastError() != ERROR_SUCCESS) {
        std::cerr << "InternetReadFile failed: " << GetLastError() << std::endl;
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hSession);
    InternetCloseHandle(hInternet);

    return ret;
}