#pragma once
#if defined(__linux__)

#include <unistd.h>

#include "../linux-extension.h"

int changeDir(fs::path dir);

#endif