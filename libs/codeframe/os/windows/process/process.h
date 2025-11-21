#pragma once
#if defined(_WIN32) || defined(_WIN64)

#include <future>
#include <iostream>

#include "../windows-extension.h"

string getProcessID();

bool runProcess(const string &cmd, bool detached = false);
void runProcessDetached(const string &exePath);

bool streamProcess(
    const string &cmd,
    const std::function<void(std::string_view)> &onChunk,
    uint32_t *outExitCode = nullptr);

string formatOutputBuffer(const char *data, size_t len, bool flush = false);

fs::path getExecutablePath();
fs::path getExecutableDir();

#endif