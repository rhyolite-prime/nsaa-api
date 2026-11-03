//
// Created by Emmanuel Addo-Odame on 30/04/2026.
//

#include "DateUtils.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace raffle::utils {

std::string DateUtils::normalizeDateString(std::string dateStr) {
    if (dateStr.empty())
        return dateStr;
    std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
    if (dateStr.length() == 16) { // YYYY-MM-DD HH:MM
        dateStr += ":00";
    }
    return dateStr;
}

bool DateUtils::isValidDateParam(const std::string &dateStr) {
    if (dateStr.empty())
        return true;
    // Expect at least YYYY-MM-DD with valid separator positions.
    if (dateStr.size() < 10 || dateStr[4] != '-' || dateStr[7] != '-')
        return false;
    auto isDigits = [](const std::string &s, size_t pos, size_t len) {
        for (size_t i = pos; i < pos + len && i < s.size(); ++i)
            if (!std::isdigit(static_cast<unsigned char>(s[i])))
                return false;
        return true;
    };
    if (!isDigits(dateStr, 0, 4) || !isDigits(dateStr, 5, 2) || !isDigits(dateStr, 8, 2))
        return false;
    int month = std::stoi(dateStr.substr(5, 2));
    int day = std::stoi(dateStr.substr(8, 2));
    if (month < 1 || month > 12 || day < 1 || day > 31)
        return false;
    // If a time portion is present it must start with 'T' or ' '.
    if (dateStr.size() > 10 && dateStr[10] != 'T' && dateStr[10] != ' ')
        return false;
    return true;
}

std::string DateUtils::formatQuartzDate(const trantor::Date &date) {
    auto seconds = static_cast<time_t>(date.microSecondsSinceEpoch() / 1000000);
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);

    char buf[32];
    strftime(buf, sizeof(buf), "%d-%m-%YT%H:%M:%S", &tm_time);
    return std::string(buf);
}

} // namespace raffle::utils
