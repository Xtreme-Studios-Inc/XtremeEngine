#pragma once
#if defined(_WIN32) || defined(_WIN64)

#include <direct.h>

#include "../windows-extension.h"

int changeDir(fs::path dir);

#endif