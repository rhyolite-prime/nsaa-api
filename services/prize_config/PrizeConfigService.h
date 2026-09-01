//
// Created by Emmanuel Addo-Odame on 18/08/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PRIZECONFIGSERVICE_H
#define CHOPMONEYRAFFLEAPI_PRIZECONFIGSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/PrizeCategoryDto.h"
#include "dto/PrizeDto.h"

namespace raffle::services {

    class PrizeConfigService {
    public:

        drogon::Task<dto::BaseApiResponse> getAllCategories(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createCategory(const dto::PrizeCategoryDto &dto);

        drogon::Task<dto::BaseApiResponse> updateCategory(const dto::PrizeCategoryDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteCategory(const std::string &id);

        //prizes ...

        drogon::Task<dto::BaseApiResponse> getAllPrizes(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> getCategoryPrizes(const std::string &id);

        drogon::Task<dto::BaseApiResponse> createPrize(const dto::PrizeDto &dto);

        drogon::Task<dto::BaseApiResponse> updatePrize(const dto::PrizeDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deletePrize(const std::string &id);

    };

}
#endif //CHOPMONEYRAFFLEAPI_PRIZECONFIGSERVICE_H
