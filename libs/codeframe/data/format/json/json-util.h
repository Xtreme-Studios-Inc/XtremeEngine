#pragma once

#include <fstream>
#include <optional>

#include "json.h"

#include "../../../cli/cli.h"
#include "../../../core/base.h"
#include "../../../file-system/file-system.h"

namespace fs = std::filesystem;

// CREATE
void createJsonFile(const string &path, const cf::json &defaultContent);

// UPDATE
void updateJsonFile(const fs::path &filePath, const cf::ordered_json &jsonData);

/**
 * @brief Get a specific JSON key
 *
 * @param jsonData
 * @param key
 * @return cf::json
 */
cf::json getJsonValue(cf::json jsonData, string key);

/**
 * @brief Get JSON Key or return empty json object
 *
 * @tparam JSON
 * @param json
 * @param key
 * @return cf::json
 */
template <class JSON> cf::json jsonGetValue(const JSON &json, string key) {
    if (json.contains(key))
        return json[key];
    return cf::json();
}

template <class JSON> cf::json jsonExpectValue(const JSON &json, string key) {
    if (!json.contains(key)) {
        console.error("Missing Field: " + key);
        return cf::json();
    } else
        return json[key];
}

/**
 * @brief Always return an array, even if its not an array we get an empty
 */
template <class T, class JSON> vector<T> ensureArray(const JSON &json) {
    if (json.is_array())
        return json.template get<std::vector<T>>();

    return {}; // return empty vector if null or missing
}

cf::ordered_json getJsonValues(cf::ordered_json &jsonData, vector<string> &keys);

cf::json loadJsonFile(fs::path path);
std::optional<cf::ordered_json> readJsonFile(fs::path path);

vector<string> getJsonKeys(cf::ordered_json &jsonData);

// Adds a string value to a JSON object only if it exists and is not empty
template <typename T> void setJsonItem(cf::ordered_json &json, const std::string &key, const std::optional<T> &value) {
    if (value && !value->empty())
        json[key] = *value;
}