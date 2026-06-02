//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_FAQSERVICE_H
#define CHOPMONEYRAFFLEAPI_FAQSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/FaqDto.h"


namespace raffle::services {

    class FaqService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::FaqDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::FaqDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteFaq(const std::string &id);
    };

}
#endif //CHOPMONEYRAFFLEAPI_FAQSERVICE_H