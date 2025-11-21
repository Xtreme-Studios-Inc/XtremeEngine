#include "os-info.h"

OSInfo getOSInfo() {

    OSInfo info;

    // --- Platform detection ---
#if defined(_WIN32)
    info.platform = "windows";
#elif defined(__APPLE__)
    info.platform = "macOS";
#elif defined(__linux__)
    info.platform = "linux";
#else
    info.platform = "unknown";
#endif

    // --- Architecture detection ---
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
    info.architecture = "x86_64";
#elif defined(_M_IX86) || defined(__i386__)
    info.architecture = "x86";
#elif defined(_M_ARM64) || defined(__aarch64__)
    info.architecture = "aarch64";
#elif defined(_M_ARM) || defined(__arm__)
    info.architecture = "aarch64";
#else
    info.architecture = "unknown";
#endif

    return info;
}