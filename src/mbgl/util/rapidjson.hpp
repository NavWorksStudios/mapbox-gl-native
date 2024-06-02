#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace mbgl {

using JSONDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
using JSONValue = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;

std::string formatJSONParseError(const JSONDocument&);

} // namespace mbgl
