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

    char buf[32];
    strftime(buf, sizeof(buf), "%d-%m-%YT%H:%M:%S", &tm_time);
    return std::string(buf);
}

} // namespace raffle::utils
