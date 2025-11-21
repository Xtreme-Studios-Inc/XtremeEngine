#include "file-system.h"

// --- DIRECTORY OPERATIONS ---
#pragma region

void dirEnsureExists(const fs::path &dirPath) {
    // CHECK IF THE DIRECTORY EXISTS
    if (fs::exists(dirPath) && fs::is_directory(dirPath))
        return;
    else if (!fs::create_directories(dirPath))
        console.error("Failed To Create Directory: " + dirPath.generic_string());
}

bool dirIsEmpty(fs::path path) { return fs::directory_iterator(path) == fs::directory_iterator(); }

void dirCopyRecursive(const fs::path &src, const fs::path &dest, bool dryRun) {
    if (!fs::exists(src) || !fs::is_directory(src))
        throw std::invalid_argument("Source directory does not exist or is not a directory.");

    if (dryRun) {
        console.logLnM("Copying directory: " + src.string() + " to " + dest.string());
    } else {
        fs::create_directories(dest);
    }

    for (const auto &dirEntry : fs::directory_iterator(src)) {
        const auto &path = dirEntry.path();
        if (fs::is_directory(path)) {
            dirCopyRecursive(path, dest / path.filename(), dryRun);
        } else if (fs::is_regular_file(path)) {
            if (dryRun) {
                console.logLn("Copying file: " + path.string(), {YELLOW});
                continue;
            }

            fs::copy_file(path, dest / path.filename(), fs::copy_options::skip_existing);
        } else {
            console.error("Unsupported file type: " + path.string());
        }
    }
}

bool dirRemove(fs::path path) {
    std::error_code error;
    fs::remove_all(path, error);

    if (error) {
        console.error("Path deletion failed: " + error.message());
        return FAILURE;
    }

    return SUCCESS;
}

// Recursively replaces strings in all text files within the given directory
void replaceStringRecursive(const fs::path &root, const StringReplaceMap &replaceMap) {
    for (auto &entry : fs::recursive_directory_iterator(root)) {
        if (fs::is_regular_file(entry.path()) && isTextFile(entry.path())) {
            // Read file content
            std::ifstream fileIn(entry.path(), std::ios::in | std::ios::binary);
            if (!fileIn)
                continue;

            std::ostringstream buffer;
            buffer << fileIn.rdbuf();
            string content = buffer.str();
            fileIn.close();

            // Replace content
            string newContent = replaceAllInMap(content, replaceMap);

            // Only overwrite if content changed
            if (newContent != content) {
                std::ofstream fileOut(entry.path(), std::ios::out | std::ios::binary | std::ios::trunc);
                fileOut << newContent;
                fileOut.close();
            }
        }
    }
}

void forEachDirEntry(const fs::path &path, std::function<void(const fs::directory_entry &)> entryFunc) {
    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            entryFunc(entry);
        }
    }
}

/**
 * @brief Iterates over all *directories* in a given path and calls a function.
 *
 * This is a non-recursive iteration.
 *
 * @param path The path to iterate over.
 * @param dirFunc The function to call for each directory entry.
 */
void forEachDir(const fs::path &path, std::function<void(const fs::directory_entry &)> dirFunc) {
    try {
        for (const auto &entry : fs::directory_iterator(path)) {
            // The only change is here:
            if (entry.is_directory()) {
                dirFunc(entry);
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error iterating directory: " << e.what() << std::endl;
    }
}

void forEachDirEntryRecurse(
    const fs::path &path,
    const vector<fs::path> &ignoredPaths,
    std::function<void(const fs::directory_entry &)> entryFunc,
    const fs::path &rootPath) {

    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            bool skip = false;
            for (auto &ignored : ignoredPaths) {
                fs::path fullIgnoredPath = rootPath / ignored;

                if (entry.path().generic_string().contains(fullIgnoredPath.generic_string())) {
                    skip = true;
                    break;
                }
            }
            if (skip)
                continue;
            forEachDirEntryRecurse(entry, ignoredPaths, entryFunc);
        } else if (entry.is_regular_file()) {
            entryFunc(entry);
        }
    }
}

PathGroup listAllFiles(const fs::path &root) {
    PathGroup files;
    if (!fs::exists(root) || !fs::is_directory(root))
        return files;

    for (const auto &entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
        if (entry.is_regular_file()) {
            files.insert(fs::relative(entry.path(), root)); // relative to root
        }
    }
    return files;
}

#pragma endregion