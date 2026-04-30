//
// Created by Emmanuel Addo-Odame on 30/04/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_STRINGUTILS_H
#define CHOPMONEYRAFFLEAPI_STRINGUTILS_H

#include <string>
#include <json/json.h>

namespace raffle::utils {

class StringUtils {
public:
    static std::string toCamelCase(const std::string &snake);
    static Json::Value snakeCaseToCamelCase(const Json::Value &val);
};

} // namespace raffle::utils

#endif //CHOPMONEYRAFFLEAPI_STRINGUTILS_H
