//
// Created by Emmanuel Addo-Odame on 07/03/2026.
//

#include "IdGeneratorUtils.h"
#include <random>
#include <sstream>


namespace raffle::utils {

    std::string IdGeneratorUtils::generateGuid() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 15);

        auto hexDigit = [&]() {
            int v = dist(gen);
            std::stringstream ss;
            ss << std::hex << std::nouppercase << v;
            return ss.str();
        };

        std::stringstream guid;

        // 8-4-4-4-12 pattern
        int groups[] = {8, 4, 4, 4, 12};
        for (int i = 0; i < 5; ++i) {
            if (i > 0)
                guid << "-";
            for (int j = 0; j < groups[i]; ++j) {
                guid << hexDigit();
            }
        }
        return guid.str();
    }

    std::string IdGeneratorUtils::generateRandomSixDigit() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(100000, 999999);
        int num = dist(gen);
        return std::to_string(num);
    }

    std::string IdGeneratorUtils::generateRandomDigit(int length) {
        if (length <= 0) {
            throw std::invalid_argument("Length must be greater than 0");
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        // For lengths greater than 9, we need to handle larger numbers
        if (length > 9) {
            // Use string building for very large numbers
            std::uniform_int_distribution<int> dist(0, 9);
            std::string result;

            // First digit cannot be zero
            std::uniform_int_distribution<int> firstDist(1, 9);
            result += std::to_string(firstDist(gen));

            // Generate remaining digits
            for (int i = 1; i < length; ++i) {
                result += std::to_string(dist(gen));
            }

            return result;
        }

        // For lengths 1-9, use integer arithmetic
        int minVal = std::pow(10, length - 1);
        int maxVal = std::pow(10, length) - 1;

        std::uniform_int_distribution<int> dist(minVal, maxVal);
        int num = dist(gen);
        return std::to_string(num);
    }
    std::string IdGeneratorUtils::generateAlphanumericId(size_t length) {
        static const char charset[] = "0123456789"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

        std::string id;
        id.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            id += charset[dist(gen)];
        }
        return id;
    }

}

