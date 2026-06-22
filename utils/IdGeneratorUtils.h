//
// Created by Emmanuel Addo-Odame on 07/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_IDGENERATORUTILS_H
#define CHOPMONEYRAFFLEAPI_IDGENERATORUTILS_H


#include <string>


namespace raffle::utils {

    class IdGeneratorUtils {
    public:
        /**
         * @brief Generates a random GUID in the format 8-4-4-4-12
         * @return A random GUID string
         */
        static std::string generateGuid();

        /**
         * @brief Generates a random 6-digit number as a string
         * @return A random 6-digit number string (100000-999999)
         */
        static std::string generateRandomSixDigit();

        /**
         * @brief Generates a random 6-digit number as a string
         * @return A random N-digit number string
         */
        static std::string generateRandomDigit(int length);

        /**
         * @brief Generates a random alphanumeric string of a given length
         * @param length The length of the string to generate
         * @return A random alphanumeric string
         */
        static std::string generateAlphanumericId(size_t length = 6);
    };

}



#endif //CHOPMONEYRAFFLEAPI_IDGENERATORUTILS_H