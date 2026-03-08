//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_RNGLOGSERVICE_H
#define CHOPMONEYRAFFLEAPI_RNGLOGSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

    class RngLogService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);


    };

}

#endif //CHOPMONEYRAFFLEAPI_RNGLOGSERVICE_H