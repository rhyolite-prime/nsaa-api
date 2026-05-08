//
// Created by Emmanuel Addo-Odame on 07/05/2026.
//

#include "DatabaseUtils.h"
#include <sstream>
#include <iomanip>

namespace raffle::utils {

std::string DatabaseUtils::formatDecimal(double val, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << val;
    return ss.str();
}

std::string DatabaseUtils::vectorToPgArray(const std::vector<std::string>& vec) {
    if (vec.empty()) return "{}";
    std::string result = "{";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::string val = vec[i];
        // Basic escaping for quotes if needed
        size_t pos = 0;
        while ((pos = val.find("\"", pos)) != std::string::npos) {
            val.replace(pos, 1, "\\\"");
            pos += 2;
        }
        result += "\"" + val + "\"";
        if (i < vec.size() - 1) result += ",";
    }
    result += "}";
    return result;
}

} // namespace raffle::utils
