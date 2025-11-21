#pragma once

#include "../../core/base.h"

struct OSInfo {
    string platform;
    string architecture;
};

OSInfo getOSInfo();