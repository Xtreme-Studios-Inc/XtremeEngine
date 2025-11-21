#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "../data/data.h"
#include "../os/os.h"
#include "../string-util/string.h"
#include "binary.h"
#include "fs-types.h"

namespace fs = std::filesystem;

using PathGroup = std::unordered_set<fs::path>;

// FILE SYSTEM STRUCTS

// FILE OPERATIONS
bool containsFile(const fs::path &folder, const string &targetFile);
unsigned int countFiles(const fs::path &directoryPath);

optional<fs::path> fileSearchUpwards(const fs::path &startDir, const string &filename);

optional<fs::path> fileSearchDownwards(const fs::path &startDir, const string &filename);

void filesRenameRecursive(const fs::path &root, const StringReplaceMap &replaceMap);

// DIRECTORY OPERATIONS
void dirEnsureExists(const fs::path &dirPath);
bool dirIsEmpty(fs::path path);
bool dirRemove(fs::path path);

void dirCopyRecursive(const fs::path &src, const fs::path &dest, bool dryRun = false);

void replaceStringRecursive(const fs::path &root, const StringReplaceMap &replaceMap);

void forEachDirEntry(const fs::path &path, std::function<void(const fs::directory_entry &)> entryFunc);
void forEachDirEntryRecurse(
    const fs::path &path,
    const vector<fs::path> &ignoredPaths,
    std::function<void(const fs::directory_entry &)> entryFunc,
    const fs::path &rootPath = fs::path());
void forEachDir(const fs::path &path, std::function<void(const fs::directory_entry &)> dirFunc);

PathGroup listAllFiles(const fs::path &root);

// JSON

cf::ordered_json fileToJson(const fs::path &path);
cf::ordered_json directoryToJson(const fs::path &path, bool verbose);
// File System to Json
cf::ordered_json fileSystemToJson(const fs::path &path, bool verbose = false);

// --- Path ---
#pragma region
string fileName(const string &path);
string fileName(const fs::path &path);

string extension(const string &path);
string extension(const fs::path &path);

fs::path pathRelativeTo(const vector<fs::path> &paths, const fs::path &basePath = fs::path());
// string pathTrimFolders(const string& originalPath, int n);
fs::path pathTrimFolders(const fs::path &path, int n);
string pathRemoveExt(const string &fileName);
#pragma endregion

// --- File ---
#pragma region
string readFile(const string &filePath);

bool writeFile(const string &str, const string &filePath, bool dryRun = false, bool verbose = false);

void copyFile(const fs::path &src, const string &fileName, const fs::path &dest);

bool copyFileT(const fs::path &srcPath, const fs::path &destPath);

string fileFindReplace(const string &filePath, const string &oldString, const string &newString);

// FILE VALIDATIONS
bool isTextFile(const fs::path &path);
#pragma endregion
