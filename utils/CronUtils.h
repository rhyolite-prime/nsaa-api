//
// Created by Emmanuel Addo-Odame on 29/04/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CRONUTILS_H
#define CHOPMONEYRAFFLEAPI_CRONUTILS_H

#include <string>
#include <trantor/utils/Date.h>

namespace raffle::utils {

class CronUtils {
public:
    /**
     * @brief Builds a cron expression from a trantor::Date object.
     * Format: 0 <min> <hour> <day> <MMM> ? <year>
     * @param date The date to convert to a cron expression.
     * @return A cron expression string.
     */
    static std::string buildCronExpression(const trantor::Date &date);
};

} // namespace raffle::utils

#endif //CHOPMONEYRAFFLEAPI_CRONUTILS_H
