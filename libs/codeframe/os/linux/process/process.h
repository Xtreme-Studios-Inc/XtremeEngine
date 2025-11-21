#pragma once

#if defined(__linux__)
#include "../linux-extension.h"

#include <unistd.h>

string getProcessID();

// bool runProcess(const char *cmd);
bool runProcess(const string &cmd, bool detached = false);
void runProcessDetached(const string &exePath);
bool streamProcess(
    const string &cmd,
    const std::function<void(std::string_view)> &onChunk,
    uint32_t *outExitCode = nullptr);

fs::path getExecutablePath();
fs::path getExecutableDir();

string formatOutputBuffer(const char *data, size_t len, bool flush = false);

#endif