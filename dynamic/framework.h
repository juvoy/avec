#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <shlobj_core.h>
#include <filesystem>
#include <fstream>

#include "request.h"
#include "crypter.h"

#include <vector>
#include <regex>