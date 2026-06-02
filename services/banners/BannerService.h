//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_BANNERSERVICE_H
#define CHOPMONEYRAFFLEAPI_BANNERSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/BannerDto.h"

namespace raffle::services {

    class BannerService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::BannerDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::BannerDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteBanner(const std::string &id);

    };
}
#endif //CHOPMONEYRAFFLEAPI_BANNERSERVICE_H