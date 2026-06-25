//
// Created by Emmanuel Addo-Odame on 12/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PASSWORDUTILS_H
#define CHOPMONEYRAFFLEAPI_PASSWORDUTILS_H
#include <string>

namespace raffle::utils {

    class PasswordUtils {
    public:
        /**
         * @brief Generates a random alphanumeric password
         * @param length The length of the password to generate (default: 8)
         * @return A random password string
         */
        static std::string generateRandomPassword(int length = 8);
        static std::string normalizeBcryptHash(std::string hash);
    };

}
#endif //CHOPMONEYRAFFLEAPI_PASSWORDUTILS_H