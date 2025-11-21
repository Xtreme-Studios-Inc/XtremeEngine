#include "process.h"

#if defined(_WIN32) || defined(_WIN64)

string getProcessID() {
    DWORD pid = GetCurrentProcessId();
    return toString(pid);
}

// Cleans/clusters compiler output by lines. Call per chunk; returns text to
// log. Pass flush=true once after the read-loop to flush any trailing partial
// line.
string formatOutputBuffer(const char *data, size_t len, bool flush) {
    static string pending; // holds partial line between calls
    string out;

    if (data && len)
        pending.append(data, len);

    auto handleLine = [&](std::string_view line) {
        // trim CR/LF
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
            line.remove_suffix(1);
        if (line.empty())
            return;

        // ------- simple routing rules (tweak as you like) -------
        // suppress super-noisy include backtraces unless you want them
        if (line.rfind("In file included from", 0) == 0)
            return;

        if (line.find("note:") != string::npos)
            return;

        // promote final clang summary
        if (line.find(" warnings and ") != std::string_view::npos && line.find(" error") != std::string_view::npos &&
            line.find(" generated.") != std::string_view::npos) {
            out.append(line).append("\n");
            return;
        }

        if (line.find("error:") != string::npos) {
            out.append(red(string(line))).append("\n");
            return;
        }

        if (line.find("warning:") != string::npos) {
            out.append(yellow(string(line))).append("\n");
            return;
        }

        // keep warnings/errors (and everything else by default)
        out.append(line).append("\n");
    };

    // emit complete lines
    size_t start = 0;
    for (;;) {
        size_t nl = pending.find('\n', start);
        if (nl == string::npos)
            break;
        handleLine(std::string_view(pending).substr(start, nl - start + 1));
        start = nl + 1;
    }
    if (start > 0)
        pending.erase(0, start);

    // flush trailing partial line at end
    if (flush && !pending.empty()) {
        handleLine(std::string_view(pending));
        pending.clear();
    }

    return out;
}

bool runProcess(const string &cmd, bool detached) {

    SECURITY_ATTRIBUTES securityAttributes {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = nullptr,
        .bInheritHandle = TRUE};

    HANDLE readHandle, writeHandle;
    if (!CreatePipe(&readHandle, &writeHandle, &securityAttributes, 0)) {
        console.error("Error :Failed to create pipe - " + toString(GetLastError()));
        return false;
    }
    SetHandleInformation(readHandle, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW startupInfo {
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES,
        .hStdOutput = writeHandle,
        .hStdError = writeHandle};

    PROCESS_INFORMATION processInfo;

    // Convert cmd to wide string
    size_t commandLength = cmd.size();
    std::wstring wcmd(commandLength, L'\0');
    size_t convertedChars = 0;

    mbstowcs_s(&convertedChars, &wcmd[0], commandLength + 1, cmd.c_str(), _TRUNCATE);

    DWORD creationFlags = 0;
    BOOL inheritHandles = TRUE;

    if (detached) {
        // Make the process run independently of this one
        // creationFlags = DETACHED_PROCESS | CREATE_NO_WINDOW;
        // creationFlags = DETACHED_PROCESS | CREATE_NEW_CONSOLE;
        creationFlags = CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
        inheritHandles = FALSE; // detached process shouldn't share our handles
    }

    if (!CreateProcessW(0, wcmd.data(), 0, 0, inheritHandles, creationFlags, 0, 0, &startupInfo, &processInfo)) {
        console.error("Error: Failed to start process - " + toString(GetLastError()));
        return false;
    }
    CloseHandle(writeHandle); // Close write-end of the pipe

    // Capture stdout/stderr output (only if attached)
    if (!detached) {
        std::string outputBuffer(4096, '\0');
        DWORD bytesRead = 0;
        std::string processOutput;

        while (true) {
            if (ReadFile(readHandle, outputBuffer.data(), (DWORD)outputBuffer.size(), &bytesRead, nullptr) &&
                bytesRead > 0) {
                std::string_view strView(outputBuffer.data(), bytesRead);
                std::string cleaned = formatOutputBuffer(strView.data(), strView.size(), false);

                if (!cleaned.empty()) {
                    console.log(cleaned);
                    processOutput += cleaned;
                }
            } else if (GetLastError() == ERROR_BROKEN_PIPE) {
                break; // Normal termination
            }
        }

        // Wait for process to finish
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        // Get Exit Code
        DWORD exitCode;
        if (GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
            if (exitCode != 0)
                console.error("Process exited with code: " + toString(exitCode));
        } else {
            console.error("Faulty Command: " + cmd);
            console.error("Failed to get exit code. Error: " + toString(GetLastError()));
        }

        CloseHandle(readHandle);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        if (exitCode != 0) {
            console.error("Error: Process failed. Command output:\n" + processOutput);
            return false;
        }
    } else {
        // Detached — don't wait or read pipe
        CloseHandle(readHandle);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }

    return true;
}

// Helper function to convert std::string to std::wstring
std::wstring s2ws(const string &str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Converts a std::wstring (UTF-16) to a std::string (UTF-8)
std::string ws2s(const std::wstring &wstr) {
    if (wstr.empty())
        return std::string();

    // Get the required buffer size (passing 0 for last 2 args)
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    // Create the string and perform the conversion
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

void runProcessDetached(const std::string &exePath) {
    // 1. Convert path to wide string
    std::wstring wExePath = s2ws(exePath);

    // 2. Create a mutable copy for lpCommandLine
    //    CreateProcessW can modify this string buffer
    wchar_t szCmdline[MAX_PATH];
    wcscpy_s(szCmdline, wExePath.c_str());

    STARTUPINFOW startupInfo; // <-- W for Wide

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    // 3. Set the "Show Window" flags
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));

    BOOL success = CreateProcessW(
        NULL,      // lpApplicationName (use NULL, pass in command line)
        szCmdline, // lpCommandLine (mutable path to exe)
        NULL,      // Process handle not inheritable
        NULL,      // Thread handle not inheritable
        FALSE,     // Set handle inheritance to FALSE
        CREATE_NEW_PROCESS_GROUP,
        NULL,         // Use parent's environment block
        NULL,         // Use parent's starting directory
        &startupInfo, // Pointer to STARTUPINFOW structure
        &processInfo  // Pointer to PROCESS_INFORMATION structure
    );

    if (success) {
        console.logLnS("Successfully launched detached process.");
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else
        console.error("Failed to create process. Error: " + toString(GetLastError()));
}

bool streamProcess(const string &cmd, const std::function<void(std::string_view)> &onChunk, uint32_t *outExitCode) {
    SECURITY_ATTRIBUTES securityAttributes {};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;

    HANDLE readHandle = nullptr, writeHandle = nullptr;
    if (!CreatePipe(&readHandle, &writeHandle, &securityAttributes, 0)) {
        console.error("CreatePipe failed: " + toString(GetLastError()));
        return false;
    }
    // Child inherits writeHandle (stdout/stderr). Parent must not inherit
    // readHandle.
    SetHandleInformation(readHandle, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW startupInfo {};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    startupInfo.wShowWindow = SW_HIDE;
    startupInfo.hStdOutput = writeHandle;
    startupInfo.hStdError = writeHandle;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE); // or nullptr if you don’t need stdin

    PROCESS_INFORMATION processInfo {};
    // Convert UTF-8 to UTF-16 and ensure writable, null-terminated buffer
    vector<wchar_t> wcmd;
    {
        int n = MultiByteToWideChar(CP_UTF8, 0, cmd.c_str(), -1, nullptr, 0);
        if (n <= 0) {
            console.error("UTF-16 conversion failed");
            CloseHandle(readHandle);
            CloseHandle(writeHandle);
            return false;
        }
        wcmd.resize(n);
        MultiByteToWideChar(CP_UTF8, 0, cmd.c_str(), -1, wcmd.data(), n);
    }

    if (!CreateProcessW(
            /*lpApplicationName*/ nullptr,
            /*lpCommandLine   */ wcmd.data(), // mutable buffer required
            /*proc attrs      */ nullptr,
            /*thread attrs    */ nullptr,
            /*inherit handles */ TRUE,
            /*flags           */ 0, // add CREATE_NO_WINDOW if it's a console tool
            /*env             */ nullptr,
            /*cwd             */ nullptr,
            /*startup info    */ &startupInfo,
            /*proc info       */ &processInfo)) {
        console.error("CreateProcessW failed: " + toString(GetLastError()));
        CloseHandle(readHandle);
        CloseHandle(writeHandle);
        return false;
    }

    // Parent must close its copy of the write end so ReadFile can see EOF.
    CloseHandle(writeHandle);

    // Read stdout/stderr streamingly
    string buf;
    buf.resize(8192);
    DWORD bytesRead = 0;

    for (;;) {
        BOOL ok = ReadFile(readHandle, buf.data(), (DWORD)buf.size(), &bytesRead, nullptr);
        if (ok && bytesRead > 0) {
            std::string_view sv(buf.data(), bytesRead);

            // Optional: clean/transform the chunk as you already do
            // std::string cleaned = formatOutputBuffer(sv.data(), sv.size());
            // if (!cleaned.empty()) onChunk(cleaned);

            onChunk(sv); // raw chunk
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_BROKEN_PIPE) {
                // child closed its end -> normal EOF
                break;
            } else if (err == ERROR_MORE_DATA) {
                // continue reading; the pipe has more data
                continue;
            } else {
                console.error("ReadFile failed: " + toString(err));
                // Some other read error or no data available
                break;
            }
        }
    }

    // Wait for completion and get exit code
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD exitCode = 0;
    if (!GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
        console.error("GetExitCodeProcess failed: " + toString(GetLastError()));
    }
    if (outExitCode)
        *outExitCode = exitCode;

    CloseHandle(readHandle);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return exitCode == 0;
}

fs::path getExecutablePath() {
    // UTF-16 → fs::path handles wide natively on Windows
    std::wstring filePath(MAX_PATH, L'\0');
    size_t pathLength = GetModuleFileNameW(nullptr, filePath.data(), static_cast<size_t>(filePath.size()));
    if (pathLength == 0)
        throw std::runtime_error("GetModuleFileNameW failed");
    // If truncated, grow and retry.
    while (pathLength == filePath.size()) {
        filePath.resize(filePath.size() * 2);
        pathLength = GetModuleFileNameW(nullptr, filePath.data(), static_cast<size_t>(filePath.size()));
        if (pathLength == 0)
            throw std::runtime_error("GetModuleFileNameW failed (retry)");
    }
    filePath.resize(pathLength);
    return fs::path(filePath);
}

fs::path getExecutableDir() { return getExecutablePath().parent_path(); }

#endif