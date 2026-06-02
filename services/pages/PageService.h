//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PAGESERVICE_H
#define CHOPMONEYRAFFLEAPI_PAGESERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/PageDto.h"

namespace raffle::services {

    class PageService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::PageDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::PageDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deletePage(const std::string &id);

    };


}
#endif //CHOPMONEYRAFFLEAPI_PAGESERVICE_H