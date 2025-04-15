#include "crypter.h"

std::vector<BYTE> base64::Decode(std::string& base64)
{
	DWORD pcbBinary;
	CryptStringToBinaryA(base64.c_str(), 0, CRYPT_STRING_BASE64, NULL, &pcbBinary, NULL, NULL);

	std::vector<BYTE> out(pcbBinary);
	CryptStringToBinaryA(base64.c_str(), 0, CRYPT_STRING_BASE64, out.data(), &pcbBinary, NULL, NULL);

	return out;
}

std::vector<BYTE> crypter::Unprotect(std::vector<BYTE>& encryptedKey)
{
	DATA_BLOB blobIn{ static_cast<DWORD>(encryptedKey.size()), const_cast<BYTE*>(encryptedKey.data()) };
	DATA_BLOB blobOut{};

	if (CryptUnprotectData(&blobIn, nullptr, nullptr, nullptr, nullptr, 0, &blobOut)) {
		std::vector<BYTE> result(blobOut.pbData, blobOut.pbData + blobOut.cbData);

		LocalFree(blobOut.pbData);
		return result;
	}

	return {};
}

std::string crypter::Decrypt(std::vector<BYTE>& key, std::vector<BYTE>& nonce, std::vector<BYTE>& ciphertext, const std::vector<BYTE>& tag)
{
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	BCRYPT_KEY_HANDLE hKey = nullptr;
	NTSTATUS status;

	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0);
	if (!BCRYPT_SUCCESS(status)) return "";

	status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_GCM,
		sizeof(BCRYPT_CHAIN_MODE_GCM), 0);
	if (!BCRYPT_SUCCESS(status)) return "";

	status = BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, (PUCHAR)key.data(), (ULONG)key.size(), 0);
	if (!BCRYPT_SUCCESS(status)) return "";

	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
	BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
	authInfo.pbNonce = const_cast<PUCHAR>(nonce.data());
	authInfo.cbNonce = (ULONG)nonce.size();
	authInfo.pbTag = const_cast<PUCHAR>(tag.data());
	authInfo.cbTag = (ULONG)tag.size();

	std::vector<BYTE> plaintext(ciphertext.size());

	ULONG result = 0;
	status = BCryptDecrypt(hKey, (PUCHAR)ciphertext.data(), (ULONG)ciphertext.size(), &authInfo, nullptr, 0, plaintext.data(), (ULONG)plaintext.size(), &result, 0);

	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);

	if (!BCRYPT_SUCCESS(status)) return "";

	return std::string(plaintext.begin(), plaintext.begin() + result);
}