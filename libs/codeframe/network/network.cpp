#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib/httplib.h"

#include "network.h"

#include <optional>
#include <string>

// Server server;

// std::optional<string> Server::get(const string &url, string &err) {
//     // console.logLn(url);
//     const size_t protocolEndPos = url.find("://");
//     const bool https = (protocolEndPos == string::npos) ? true : url.substr(0, protocolEndPos) == "https";
//     const size_t hostStartPos = (protocolEndPos == string::npos) ? 0 : protocolEndPos + 3;
//     const size_t hostEndPos = url.find('/', hostStartPos);
//     string host = url.substr(hostStartPos, (hostEndPos == string::npos ? url.size() : hostEndPos) - hostStartPos);
//     string path = (hostEndPos == string::npos) ? "/" : url.substr(hostEndPos);

//     httplib::Headers h = {
//         { "User-Agent", "CodeFrame/1.0"            },
//         { "Accept",     "application/octet-stream" }
//     };
//     int status = 0;
//     string body;

//     auto doGet = [&](auto &cli) -> bool {
//         cli.set_follow_location(true); // fine; typically no redirects now
//         cli.enable_server_certificate_verification(true);
//         cli.set_connection_timeout(5);
//         cli.set_read_timeout(300);
//         if (auto res = cli.Get(path.c_str(), h)) {
//             status = res->status;
//             body = std::move(res->body);
//             return true;
//         }
//         return false;
//     };

//     if (https) {
//         httplib::SSLClient cli(host.c_str());
//         if (!doGet(cli)) {
//             err = "no response";
//             return std::nullopt;
//         }
//     } else {
//         httplib::Client cli(host.c_str());
//         if (!doGet(cli)) {
//             err = "no response";
//             return std::nullopt;
//         }
//     }

//     if (status == 200)
//         return body;
//     err = "HTTP " + std::to_string(status);
//     return std::nullopt;
// }