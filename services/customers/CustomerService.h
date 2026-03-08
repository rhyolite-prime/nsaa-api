//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#define CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>


namespace raffle::services {

    class CustomerService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> getStats();


    };

}
#endif //CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H