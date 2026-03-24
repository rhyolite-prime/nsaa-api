//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PRICETIERSERVICE_H
#define CHOPMONEYRAFFLEAPI_PRICETIERSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/CreatePriceTierDto.h"
#include "dto/UpdatePriceTierDto.h"

namespace raffle::services {

    class PriceTierService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createAsync(const dto::CreatePriceTierDto &dto);

        drogon::Task<dto::BaseApiResponse> updateAsync(const dto::UpdatePriceTierDto &dto);

        drogon::Task<dto::BaseApiResponse> deletePriceTier(const std::string &tierId);

    };

}
#endif //CHOPMONEYRAFFLEAPI_PRICETIERSERVICE_H