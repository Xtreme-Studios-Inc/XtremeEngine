#pragma once

#include <thread>
#include "../file-system/file-system.h"
#include "mime-type.h"

#define CF_PORT 2727

class Server {
  private:
    /* data */
  public:
    Server() = default;
    Server(const fs::path &path, int port = CF_PORT);

    ~Server();

    bool create(const fs::path &folderPath, int port = CF_PORT);
    void stop();

    std::optional<string> get(const string &url, string &err);

  private:
    fs::path folder;
    int port = 0;
    std::atomic<bool> running {false};
    std::thread serverThread;
};

extern Server server;