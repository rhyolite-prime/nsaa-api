//
// Created by Emmanuel Addo-Odame on 18/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONSERVICE_H
#define CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/TicketCollectionDto.h"

namespace raffle::services {

    class TicketCollectionService {

    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::TicketCollectionDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::TicketCollectionDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteRole(const std::string &roleId);

    };

}
#endif //CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONSERVICE_H