#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib/httplib.h"

#include "server.h"

Server server;

Server::Server(const fs::path &path, int port) { this->create(path, port); }

Server::~Server() { this->stop(); }

bool Server::create(const fs::path &folderPath, int port) {
    if (running) {
        console.error("Error: Failed to start server — instance already running on port " + toString(this->port));
        return false;
    }
    this->folder = folderPath;
    this->port = port;
    running = true;

    serverThread = std::thread([this]() {
        httplib::Server server;

        server.Get(R"(/(.*))", [this](const httplib::Request &req, httplib::Response &res) {
            fs::path file = folder / req.matches[1].str();
            if (req.matches[1].str().empty() || fs::is_directory(file))
                file = folder / "index.html";

            if (!fs::exists(file)) {
                res.status = 404;
                res.set_content("404 Not Found", "text/plain");
                return;
            }

            std::ifstream ifs(file, std::ios::binary);
            std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

            // std::string ext = file.extension().string();
            // std::string mime = "application/octet-stream";
            // if (ext == ".html")
            //     mime = "text/html";
            // else if (ext == ".js")
            //     mime = "application/javascript";
            // else if (ext == ".css")
            //     mime = "text/css";
            // else if (ext == ".json")
            //     mime = "application/json";
            // else if (ext == ".ico")
            //     mime = "image/x-icon";
            // else if (ext == ".png")
            //     mime = "image/png";
            // else if (ext == ".jpg" || ext == ".jpeg")
            //     mime = "image/jpeg";

            string mime = getMimeType(file);

            res.set_content(content, mime.c_str());
        });

        std::thread([&]() {
            while (running)
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            server.stop();
        }).detach();

        std::cout << "🌐 Serving folder: " << folder << " on http://localhost:" << this->port << std::endl;
        server.listen("0.0.0.0", this->port);
    });

    return true;
}

void Server::stop() {
    if (!running)
        return;
    running = false;
    if (serverThread.joinable())
        serverThread.join();
    std::cout << "🛑 Server stopped.\n";
}

std::optional<string> Server::get(const string &url, string &err) {
    // console.logLn(url);
    const size_t protocolEndPos = url.find("://");
    const bool https = (protocolEndPos == string::npos) ? true : url.substr(0, protocolEndPos) == "https";
    const size_t hostStartPos = (protocolEndPos == string::npos) ? 0 : protocolEndPos + 3;
    const size_t hostEndPos = url.find('/', hostStartPos);
    string host = url.substr(hostStartPos, (hostEndPos == string::npos ? url.size() : hostEndPos) - hostStartPos);
    string path = (hostEndPos == string::npos) ? "/" : url.substr(hostEndPos);

    httplib::Headers h = {
        { "User-Agent", "CodeFrame/1.0"            },
        { "Accept",     "application/octet-stream" }
    };
    int status = 0;
    string body;

    auto doGet = [&](auto &cli) -> bool {
        cli.set_follow_location(true); // fine; typically no redirects now
        cli.enable_server_certificate_verification(true);
        cli.set_connection_timeout(5);
        cli.set_read_timeout(300);
        if (auto res = cli.Get(path.c_str(), h)) {
            status = res->status;
            body = std::move(res->body);
            return true;
        }
        return false;
    };

    if (https) {
        httplib::SSLClient cli(host.c_str());
        if (!doGet(cli)) {
            err = "no response";
            return std::nullopt;
        }
    } else {
        httplib::Client cli(host.c_str());
        if (!doGet(cli)) {
            err = "no response";
            return std::nullopt;
        }
    }

    if (status == 200)
        return body;
    err = "HTTP " + std::to_string(status);
    return std::nullopt;
}