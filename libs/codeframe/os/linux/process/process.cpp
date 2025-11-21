#include "process.h"

#if defined(__linux__)

string getProcessID() {
    pid_t pid = getpid();
    return toString(pid);
}

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

// bool runProcess(const char *cmd) {
//     int status;
//     pid_t pid = fork();

//     if (pid == 0) // This is the child process
//     {
//         execl("/bin/sh", "sh", "-c", cmd, NULL);
//         exit(EXIT_FAILURE); // execl only returns on error
//     } else if (pid < 0)     // Failed to fork
//     {
//         return false; // Return false if fork failed
//     } else            // This is the parent process
//     {
//         waitpid(pid, &status, 0); // Wait for the child to finish
//         if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
//             return true; // Return true if child process exited normally with status 0
//         } else {
//             return false; // Return false if child process failed
//         }
//     }
// }

bool runProcess(const string &cmd, bool detached) { return false; }

void runProcessDetached(const string &exePath) { return; }

bool streamProcess(const string &cmd, const std::function<void(std::string_view)> &onChunk, uint32_t *outExitCode) {
    return false;
}

fs::path getExecutablePath() {
    // Linux (and many BSDs with procfs)
    vector<char> buf(4096);
    ssize_t n = readlink("/proc/self/exe", buf.data(), buf.size() - 1);
    if (n < 0)
        throw std::runtime_error("readlink(/proc/self/exe) failed");
    buf[size_t(n)] = '\0';
    return fs::path(buf.data());
}

fs::path getExecutableDir() { return getExecutablePath().parent_path(); }

#endif