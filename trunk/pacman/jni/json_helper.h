#pragma once

#include "json/json.h"

#include <string>

namespace Pacman {
namespace JsonHelper {

Json::Value ParseJson(const std::string& data);

} // JsonHelper namespace
} // Pacman namespace