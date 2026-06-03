//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_SUPPORTTICKETSERVICE_H
#define CHOPMONEYRAFFLEAPI_SUPPORTTICKETSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/SupportTicketDto.h"

namespace raffle::services {

    class SupportTicketService {

    public:

        drogon::Task<dto::BaseApiResponse> create(const dto::SupportTicketDto &dto);

    };

}
#endif //CHOPMONEYRAFFLEAPI_SUPPORTTICKETSERVICE_H