//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H
#define CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

    class SettingService {
    public:

        drogon::Task<dto::BaseApiResponse> getSettings(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> createOrUpdate();


    };

}
#endif //CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H