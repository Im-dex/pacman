#include "json_helper.h"

namespace Pacman {
namespace JsonHelper {

Json::Value ParseJson(const std::string& data)
{
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(data, root, false))
		return Json::Value::null;
	return root;
}

} // JsonHelper namespace
} // Pacman namespace