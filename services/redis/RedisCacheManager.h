//
// Created by Emmanuel Addo-Odame on 12/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_REDISCACHEMANAGER_H
#define CHOPMONEYRAFFLEAPI_REDISCACHEMANAGER_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"

namespace raffle::services {

    class RedisCacheManager {

    public:

        drogon::Task<std::string> getValue(std::string key, bool shouldRemove = false);

        drogon::Task<dto::BaseApiResponse> setValue(std::string key, std::string value);

        // Stores key → value with an expiry (seconds). Use for auth tokens / request IDs.
        drogon::Task<dto::BaseApiResponse> setValueWithTtl(std::string key, std::string value, int ttlSeconds);

        drogon::Task<dto::BaseApiResponse> removeValue(std::string key);

    };

}
#endif //CHOPMONEYRAFFLEAPI_REDISCACHEMANAGER_H