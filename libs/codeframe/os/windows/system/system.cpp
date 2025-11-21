#include "system.h"

#if defined(_WIN32) || defined(_WIN64)

int changeDir(fs::path dir) { return _chdir(dir.generic_string().c_str()); }

#endif