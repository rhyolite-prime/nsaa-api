//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TRANSACTIONAUDITSERVICE_H
#define CHOPMONEYRAFFLEAPI_TRANSACTIONAUDITSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

    class TransactionAuditService {
    public:

        drogon::Task<dto::BaseApiResponse> getStats(int pageNo, int pageSize, const std::string &query);


    };

}

#endif //CHOPMONEYRAFFLEAPI_TRANSACTIONAUDITSERVICE_H