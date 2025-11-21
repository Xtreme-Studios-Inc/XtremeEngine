#include "network.h"

string getMimeType(const fs::path &file) {
    static const std::unordered_map<string, string> mimeTable = {
        { ".html", "text/html"              },
        { ".htm",  "text/html"              },
        { ".js",   "application/javascript" },
        { ".mjs",  "application/javascript" },
        { ".css",  "text/css"               },
        { ".json", "application/json"       },
        { ".ico",  "image/x-icon"           },
        { ".png",  "image/png"              },
        { ".jpg",  "image/jpeg"             },
        { ".jpeg", "image/jpeg"             },
        { ".svg",  "image/svg+xml"          },
        { ".txt",  "text/plain"             },
        { ".wasm", "application/wasm"       },
    };

    string ext = file.extension().string();
    auto it = mimeTable.find(ext);
    if (it != mimeTable.end()) {
        return it->second;
    }
    return "application/octet-stream";
}