#pragma once
#include <string>
#include <vector>

#include <windows.h>

#include <bcrypt.h>
#include <wincrypt.h>


namespace base64 {

	std::vector<BYTE> Decode(std::string& base64);
}

namespace crypter {
	std::vector<BYTE> Unprotect(std::vector<BYTE>& encryptedKey);

	std::string Decrypt(std::vector<BYTE>& key, std::vector<BYTE>& nonce, std::vector<BYTE>& ciphertext, const std::vector<BYTE>& tag);

}