//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H
#define CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

    class DashboardService {
    public:

        drogon::Task<dto::BaseApiResponse> getStats(int pageNo, int pageSize, const std::string &query);


    };

}
#endif //CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H