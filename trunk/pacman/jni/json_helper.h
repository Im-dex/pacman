#pragma once

#include <string>

#include "base.h"
#include "json/json.h"

namespace Pacman {
namespace JsonHelper {

Json::Value ParseJson(const std::string& data);

} // JsonHelper namespace
} // Pacman namespace