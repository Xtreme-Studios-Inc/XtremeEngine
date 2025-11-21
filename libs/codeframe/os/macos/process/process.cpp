#include "process.h"

#if defined(__APPLE__)

string getProcessID() {
    pid_t pid = getpid();
    return toString(pid);
}

bool runProcess(const char *cmd) {
    int status;
    pid_t pid = fork();

    if (pid == 0) // This is the child process
    {
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(EXIT_FAILURE); // execl only returns on error
    } else if (pid < 0)     // Failed to fork
    {
        return false; // Return false if fork failed
    } else            // This is the parent process
    {
        waitpid(pid, &status, 0); // Wait for the child to finish
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return true; // Return true if child process exited normally with status 0
        } else {
            return false; // Return false if child process failed
        }
    }
}

fs::path getExecutablePath() {
    // _NSGetExecutablePath may return a path with symlinks; resolve with realpath
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    string tmp(size, '\0');
    if (_NSGetExecutablePath(tmp.data(), &size) != 0)
        throw std::runtime_error("_NSGetExecutablePath failed");
    char resolved[PATH_MAX];
    if (realpath(tmp.c_str(), resolved) == nullptr)
        return fs::path(tmp); // fallback (still fine)
    return fs::path(resolved);
}

fs::path getExecutableDir() { return getExecutablePath().parent_path(); }
#endif