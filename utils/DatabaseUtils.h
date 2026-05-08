//
// Created by Emmanuel Addo-Odame on 07/05/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DATABASEUTILS_H
#define CHOPMONEYRAFFLEAPI_DATABASEUTILS_H

#include <string>
#include <vector>

namespace raffle::utils {

class DatabaseUtils {
public:
    static std::string formatDecimal(double val, int precision = 2);
    static std::string vectorToPgArray(const std::vector<std::string>& vec);
};

} // namespace raffle::utils

#endif //CHOPMONEYRAFFLEAPI_DATABASEUTILS_H
