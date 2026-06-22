//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_COLORSERVICE_H
#define CHOPMONEYRAFFLEAPI_COLORSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/CreateColorDto.h"
#include "dto/UpdateColorDto.h"

namespace raffle::services {

    class ColorService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> listAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createAsync(const dto::CreateColorDto &dto);

        drogon::Task<dto::BaseApiResponse> updateAsync(const dto::UpdateColorDto &dto);

        drogon::Task<dto::BaseApiResponse> deleteColor(const std::string &colorId);

    };

}
#endif //CHOPMONEYRAFFLEAPI_COLORSERVICE_H