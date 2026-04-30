//
// Created by Emmanuel Addo-Odame on 30/04/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DATEUTILS_H
#define CHOPMONEYRAFFLEAPI_DATEUTILS_H

#include <string>
#include <trantor/utils/Date.h>

namespace raffle::utils {

class DateUtils {
public:
    static std::string normalizeDateString(std::string dateStr);
    static std::string formatQuartzDate(const trantor::Date &date);
};

} // namespace raffle::utils

#endif //CHOPMONEYRAFFLEAPI_DATEUTILS_H
