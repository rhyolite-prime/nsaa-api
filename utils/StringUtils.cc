//
// Created by Emmanuel Addo-Odame on 30/04/2026.
//

#include "StringUtils.h"
#include <cctype>

namespace raffle::utils {

std::string StringUtils::toCamelCase(const std::string &snake) {
    std::string camel;
    bool nextUpper = false;
    for (char c : snake) {
        if (c == '_') {
            nextUpper = true;
        } else {
            if (nextUpper) {
                camel += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                nextUpper = false;
            } else {
                camel += c;
            }
        }
    }
    return camel;
}

Json::Value StringUtils::snakeCaseToCamelCase(const Json::Value &val) {
    if (val.isObject()) {
        Json::Value camelObj(Json::objectValue);
        for (auto it = val.begin(); it != val.end(); ++it) {
            camelObj[toCamelCase(it.name())] = snakeCaseToCamelCase(*it);
        }
        return camelObj;
    } else if (val.isArray()) {
        Json::Value camelArray(Json::arrayValue);
        for (const auto &item : val) {
            camelArray.append(snakeCaseToCamelCase(item));
        }
        return camelArray;
    }
    return val;
}

} // namespace raffle::utils
