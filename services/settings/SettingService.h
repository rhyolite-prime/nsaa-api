//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H
#define CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/CreateSettingsDto.h"

namespace raffle::services {

    class SettingService {
    public:

        drogon::Task<dto::BaseApiResponse> getSettings();

        drogon::Task<dto::BaseApiResponse> createOrUpdate(const dto::CreateSettingsDto &dto);


    };

}
#endif //CHOPMONEYRAFFLEAPI_SETTINGSERVICE_H