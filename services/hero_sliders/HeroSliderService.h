//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_HEROSLIDERSERVICE_H
#define CHOPMONEYRAFFLEAPI_HEROSLIDERSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/HeroSliderDto.h"

namespace raffle::services {

    class HeroSliderService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::HeroSliderDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::HeroSliderDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteHeroSlider(const std::string &id);

    };


}
#endif //CHOPMONEYRAFFLEAPI_HEROSLIDERSERVICE_H