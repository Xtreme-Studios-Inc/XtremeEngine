#pragma once

#include <string>
#include <set>
#include <vector>

#include "type-extension.h"

using std::string, std::vector, std::set;

#define cconst constexpr

#define EXIT true
#define CONTINUE false

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define SUCCESS true
#define FAILURE false

// Standard Functions

template <typename T> string toString(const T &value) { return std::to_string(value); }

template <typename T> vector<T> setToVector(set<T> &set) {
    vector<T> newVector(set.begin(), set.end());
    return newVector;
}