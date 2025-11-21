#include "system.h"

#if defined(__linux__)

int changeDir(fs::path dir) { return chdir(dir.generic_string().c_str()); }

#endif