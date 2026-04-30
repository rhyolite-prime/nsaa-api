//
// Created by Emmanuel Addo-Odame on 30/04/2026.
//

#include "DateUtils.h"
#include <algorithm>
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

std::string DateUtils::formatQuartzDate(const trantor::Date &date) {
    auto seconds = static_cast<time_t>(date.microSecondsSinceEpoch() / 1000000);
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);

    std::ostringstream oss;
    oss << (tm_time.tm_mon + 1) << "/" << tm_time.tm_mday << "/"
        << (tm_time.tm_year + 1900) << " "
        << (tm_time.tm_hour % 12 == 0 ? 12 : tm_time.tm_hour % 12) << ":"
        << std::setfill('0') << std::setw(2) << tm_time.tm_min << ":"
        << std::setfill('0') << std::setw(2) << tm_time.tm_sec << " "
        << (tm_time.tm_hour >= 12 ? "PM" : "AM");
    return oss.str();
}

} // namespace raffle::utils
